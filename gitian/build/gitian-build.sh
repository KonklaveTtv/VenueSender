#!/bin/bash
set -e  # Stop the script on any error

# Start the ssh-agent and add the key
SSH_KEY_PATH="/home/spencerlievens/.ssh/id_rsa"
eval $(ssh-agent -s)
ssh-add $SSH_KEY_PATH

# Define version (replace this with the actual version)
VERSION="0.0.1a"

# Your Gitian public key (this should be kept safe and private)
SIGNER="spencerlievens@gmail.com"

# Define directory variables
DIR="/home/spencerlievens"
GITIAN_DIR="$DIR/gitian-builder/bin"
ETC_DIR="$DIR/gitian-builder/etc"
LIBEXEC_DIR="$DIR/gitian-builder/libexec/"
SHARE_DIR="$DIR/gitian-builder/share"
TARGET_BIN_DIR="$DIR/gitian-builder/target-bin"

# Set environment variables for the gbuild script
export USE_LXC=1
export LXC_ARCH=amd64
export LXC_SUITE=lunar
export DISTRO=ubuntu
export NPROCS=8
export VMEM=16384
export GITIAN_SUDO_USE_DEBOOTSTRAP_DIR="yes"

# Add libexec directory to PATH
export PATH="$GITIAN_DIR:$ETC_DIR:$LIBEXEC_DIR:$SHARE_DIR:$TARGET_BIN_DIR:$PATH"

# Initialize Gitian using the base image
${GITIAN_DIR}/make-base-vm --lxc --arch amd64 --suite lunar --disksize 38147

# Build and sign for each platform
for platform in linux win osx; do
    # Build
    echo "Executing: $GITIAN_DIR/gbuild $DIR/VenueSender/gitian/gitian-descriptors/gitian-${platform}.yml"
    FULL_PATH_TO_YAML=$(realpath "$DIR/VenueSender/gitian/gitian-descriptors/gitian-${platform}.yml")
    sudo -E $GITIAN_DIR/gbuild $FULL_PATH_TO_YAML
    echo "gbuild exit code: $?"

    # Sign
    $GITIAN_DIR/gsign --signer "$SIGNER" --release ${VERSION}-${platform} --destination gitian.sigs/ gitian-${platform}.yml
    
    # Verify
    $GITIAN_DIR/gverify --release ${VERSION}-${platform} --destination gitian.sigs/ gitian-${platform}.yml
done

# Publish binaries and signatures (optional)
# You can add your code here to automatically upload these to a server or repository

echo "Gitian build process completed."
