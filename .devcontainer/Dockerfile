FROM fedora:latest

RUN dnf -y install https://packages.endpoint.com/rhel/7/os/x86_64/endpoint-repo-1.7-1.x86_64.rpm && dnf -y install git && dnf -y update && dnf -y install cmake g++ qt5-*-devel && dnf clean all
