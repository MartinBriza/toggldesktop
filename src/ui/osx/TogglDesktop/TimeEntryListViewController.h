//
//  TimeEntryListViewController.h
//  Toggl Desktop on the Mac
//
//  Created by Tanel Lebedev on 19/09/2013.
//  Copyright (c) 2013 TogglDesktop developers. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class TimerEditViewController;

@interface TimeEntryListViewController : NSViewController
@property (nonatomic, strong) TimerEditViewController *timerEditViewController;
@property (nonatomic, assign, readonly) BOOL isEditorOpen;
@end
