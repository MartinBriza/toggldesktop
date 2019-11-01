#!/usr/bin/env bash

set -e

# Force using sdk 10.11
export  LDFLAGS="-mmacosx-version-min=10.11"
export   CFLAGS="$LDFLAGS"
export CXXFLAGS="$LDFLAGS"

export TAG_NAME="v7.9.99"
version=${TAG_NAME/v/}

function app_path() {
    echo $(xcodebuild -scheme TogglDesktop -workspace src/ui/osx/TogglDesktop.xcworkspace -configuration Release -showBuildSettings \
                | grep -w 'BUILT_PRODUCTS_DIR' \
                | cut -d'=' -f 2 \
                | sed -e 's/^[ \t]*//')/TogglDesktop.app
}

function dependencies() {
    make deps
}

function cocoapods() {
    make init_cocoapod
}

function app() {
    make app_release
}

function plist() {
    # Get app path
    APP_PATH=$(app_path)

    # Update the plist file (version, enable update check, UI logging to file etc)
    mkdir -p tmp 
    #go run src/branding/osx/plist.go -path="$APP_PATH" -version=$version
    awk '/CFBundleVersion/{print;getline;$0="\t<string>'"$version"'</string>"}1' $APP_PATH/Contents/Info.plist > tmp/Info.plist
    mv tmp/Info.plist $APP_PATH/Contents/Info.plist
    awk '/CFBundleShortVersionString/{print;getline;$0="\t<string>'"$version"'</string>"}1' $APP_PATH/Contents/Info.plist > tmp/Info.plist
    # Overwrite built apps plist file
    mv tmp/Info.plist $APP_PATH/Contents/Info.plist
    
    rmdir tmp
}

function sign() {
    security unlock-keychain -p 'password' build.keychain
    APP=$(app_path)
    EXECUTABLE=$APP/Contents/MacOS/TogglDesktop
    CERTIFICATE="Developer ID Application: TOGGL OU"

    echo "== check that gatekeeper is enabled =="
    spctl --status|grep "disabled" && echo "cannot continue"

    codesign --force --options runtime --deep --sign "${CERTIFICATE}" $APP/Contents/Frameworks/Sparkle.framework/Resources/Autoupdate.app
    codesign --force --options runtime --deep --sign "${CERTIFICATE}" $APP/Contents/Frameworks/Sparkle.framework/Resources/Autoupdate.app/Contents/MacOS/fileop

for filename in $APP/Contents/Frameworks/*; do
        codesign -d --force --options runtime -vvvv --verify --strict -s "${CERTIFICATE}"  -r='designated => anchor apple generic and certificate leaf[subject.OU] = "B227VTMZ94"' $filename
    done

    codesign -d --force --options runtime -vvvv --verify --strict -s "${CERTIFICATE}" -r='designated => anchor apple generic and identifier "com.toggl.toggldesktop.TogglDesktop" and certificate leaf[subject.OU] = "B227VTMZ94"' $EXECUTABLE

    codesign -d --force --options runtime -vvvv --verify --strict -s "${CERTIFICATE}" -r='designated => anchor apple generic and identifier "com.toggl.toggldesktop.TogglDesktop" and certificate leaf[subject.OU] = "B227VTMZ94"' $APP

    codesign --deep --verify --strict --verbose=4 $APP
}

function wait_while_in_progress() {
    while true; do \
        /usr/bin/xcrun altool --notarization-info `/usr/libexec/PlistBuddy -c "Print :notarization-upload:RequestUUID" $(UPLOAD_INFO_PLIST)` -u $(DEVELOPER_USERNAME) -p $(DEVELOPER_PASSWORD) --output-format xml > $(REQUEST_INFO_PLIST) ;\
        if [[ `/usr/libexec/PlistBuddy -c "Print :notarization-info:Status" $(REQUEST_INFO_PLIST)` != "in progress" ]]; then \
            break ;\
        fi ;\
        echo '\n***** Notarization - waiting 60s' ;\
        sleep 60 ;\
    done
}

function notarize() {
    BUNDLE_APP=$(dirname $(app_path))
    EXPORT_PATH=${BUNDLE_APP}/Submissions
    APP_PATH=${BUNDLE_APP}/TogglDesktop.app
    BUNDLE_ZIP=${EXPORT_PATH}/TogglDesktop.zip
    UPLOAD_INFO_PLIST=${EXPORT_PATH}/UploadInfo.plist
    REQUEST_INFO_PLIST=${EXPORT_PATH}/RequestInfo.plist
    AUDIT_INFO_JSON=${EXPORT_PATH}/AuditInfo.json
    DEVELOPER_USERNAME=${APPLE_APPID_USER}
    DEVELOPER_PASSWORD=${APPLE_APPID_PASSWORD}

    echo "Notarization" "Building a ZIP archive…"
    /usr/bin/ditto -c -k --keepParent $(APP_PATH) $(BUNDLE_ZIP)
    echo "Notarization" "Uploading for notarization…"
    /usr/bin/xcrun altool --notarize-app --primary-bundle-id "com.toggl.toggldesktop.TogglDesktop.zip" -itc_provider "B227VTMZ94" -u $(DEVELOPER_USERNAME) -p $(DEVELOPER_PASSWORD) -f $(BUNDLE_ZIP) --output-format xml > $(UPLOAD_INFO_PLIST)
    echo "Notarization" "Waiting while notarized…"
        /usr/bin/xcrun altool --notarization-info `/usr/libexec/PlistBuddy -c "Print :notarization-upload:RequestUUID" $(UPLOAD_INFO_PLIST)` -u $(DEVELOPER_USERNAME) -p $(DEVELOPER_PASSWORD) --output-format xml > $(REQUEST_INFO_PLIST)
    echo wait_while_in_progress
    echo "Notarization" "Downloading log file…"
    /usr/bin/curl -o $(AUDIT_INFO_JSON) `/usr/libexec/PlistBuddy -c "Print :notarization-info:LogFileURL" $(REQUEST_INFO_PLIST)`
    if [ `/usr/libexec/PlistBuddy -c "Print :notarization-info:Status" $(REQUEST_INFO_PLIST)` != "success" ]; then \
        false; \
    fi
    echo notify, "Notarization", "Stapling…"
    /usr/bin/xcrun stapler staple $(APP_PATH)
    echo notify, "Notarization", "✅ Done!"
}

function dmg() {
    npm i -g create-dmg
    npm create-dmg $APP_PATH
    mv *.dmg TogglDesktop.dmg
}

function rename_dmg() {
    export timestamp=$(date "+%Y-%m-%d-%H-%M-%S") 
    export escaped_version=$(echo $version | sed 's/\./_/g') 
    export installer=TogglDesktop-$escaped_version-$timestamp.dmg
    export installer_name=TogglDesktop-$escaped_version.dmg

    mv TogglDesktop.dmg $installer 
}

function debuginfo() {
    # Compress main app debug info
    export dsym_dylib=TogglDesktopLibrary.dylib-$escaped_version-$timestamp-dsym.tar.gz 
    rm -rf $dsym_dylib 
    tar cvfz $dsym_dylib $APP_PATH/../TogglDesktopLibrary.dylib.dSYM

    # Compress dynamic library debug info
    export dsym=TogglDesktop-$escaped_version-$timestamp-dsym.tar.gz 
    rm -rf $dsym 
    tar cvfz $dsym $APP_PATH/../TogglDesktop.app.dSYM
}


if [[ "$#" -ne 1 ]]; then
    dependencies
    cocoapods
    app
    plist
    sign
    #notarize
    debuginfo
    dmg
    rename_dmg
else
    $1
fi

# Update AppCast
# Upload the new version to Github releases
# Update releases.json
# Post to Slack
