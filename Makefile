# Nom de l'image
IMAGE_NAME = 420d-builder:latest

# Répertoires
PROJECT_ROOT := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
DOCKER_DIR := $(PROJECT_ROOT)/docker

ifndef INSTALL_PATH
	INSTALL_PATH := $(PROJECT_ROOT)/output
endif

.PHONY: all build run clean

# Build complet = build image + run dans container
all: run

# Construction de l'image
build:
	@echo "   ------------------------------------------------------------------"
	@echo "🔧 Building docker image if necessary; will be long the first time..."
	@echo "   ------------------------------------------------------------------"
	podman build -t $(IMAGE_NAME) -f $(DOCKER_DIR)/Dockerfile $(PROJECT_ROOT)

# Exécution du build dans le container
run: build
	@echo "   ---------------------------------"
	@echo "🚀 Execute build in the container..."
	@echo "   ---------------------------------"
	podman run --rm \
		-v $(PROJECT_ROOT):/project:Z \
		-e RELEASE -e ENABLE_DEBUG \
		$(IMAGE_NAME)

install:
	@install -D $(PROJECT_ROOT)/src/AUTOEXEC.BIN  $(INSTALL_PATH)/AUTOEXEC.BIN
	@install -D $(PROJECT_ROOT)/src/languages.ini $(INSTALL_PATH)/420D/languages.ini
	@echo "   ---------------------------------------------------------------------------"
	@echo "✅ Fichiers installed in $(INSTALL_PATH)"
	@echo "   ---------------------------------------------------------------------------"


# Nettoyage
clean:
	@echo "   -----------------------"
	@echo "🧹 Cleaning build files..."
	@echo "   -----------------------"
	$(MAKE) -C src clean || true
