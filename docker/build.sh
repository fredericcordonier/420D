#!/bin/bash
podman build -t 420d-builder -f docker/Dockerfile .
podman run --rm -v $(pwd):/project 420d-builder
