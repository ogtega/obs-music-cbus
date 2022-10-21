# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.245.2/containers/ubuntu/.devcontainer/base.Dockerfile

# [Choice] Ubuntu version (use ubuntu-22.04 or ubuntu-18.04 on local arm64/Apple Silicon): ubuntu-22.04, ubuntu-20.04, ubuntu-18.04
ARG VARIANT="jammy"
FROM mcr.microsoft.com/vscode/devcontainers/base:0-${VARIANT}

# [Optional] Uncomment this section to install additional OS packages.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends zsh cmake ccache curl git jq ninja-build pkg-config clang clang-format-13 build-essential \
    libcurl4-openssl-dev \
    libavcodec-dev libavdevice-dev libavfilter-dev libavformat-dev libavutil-dev \
    libswresample-dev libswscale-dev \
    libjansson-dev \
    libx11-xcb-dev \
    libgles2-mesa-dev \
    libwayland-dev \
    libpulse-dev \
    qt6-base-dev libqt6svg6-dev qt6-base-private-dev \
    dotnet6
