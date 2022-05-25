VERSION_GO  = version.go

_NAME       = $(shell grep -o 'AppName string = "[^"]*"' $(VERSION_GO)  | cut -d '"' -f2)
_VERSION    = $(shell grep -oE 'Version string = "[0-9]+\.[0-9]+\.[0-9]+"' $(VERSION_GO) | cut -d '"' -f2)

_HALIDE     = "generator"
_HALIDE_VER = "14.0.0"

.PHONY: vet
vet:
	go vet ./

.PHONY: test
test:
	go test -v ./...

.PHONY: setup-halide
setup-halide:
ifeq ($(shell uname),Linux)
	sudo apt install libpng-dev libjpeg-dev clang g++ binutils
else
	brew install halide
endif

.PHONY: setup-halide-runtime_linux
setup-halide-runtime_linux:
ifeq ($(shell [ -d detector/Halide-Runtime ] && echo "1"),1)
	@echo "detector/Halide-Runtime exists"
else
	curl -O -sSL https://github.com/halide/Halide/releases/download/v14.0.0/Halide-14.0.0-x86-64-linux-6b9ed2afd1d6d0badf04986602c943e287d44e46.tar.gz
	tar xzf Halide-14.0.0-x86-64-linux-6b9ed2afd1d6d0badf04986602c943e287d44e46.tar.gz
	mv Halide-14.0.0-x86-64-linux ./Halide-Runtime
	rm Halide-14.0.0-x86-64-linux-6b9ed2afd1d6d0badf04986602c943e287d44e46.tar.gz
endif

.PHONY: setup-halide-runtime_darwin
setup-halide-runtime_darwin:
ifeq ($(shell [ -d detector/Halide-Runtime ] && echo "1"),1)
	@echo "detector/Halide-Runtime exists"
else
	curl -O -sSL https://github.com/halide/Halide/releases/download/v14.0.0/Halide-14.0.0-x86-64-osx-6b9ed2afd1d6d0badf04986602c943e287d44e46.tar.gz
	tar xzf Halide-14.0.0-x86-64-osx-6b9ed2afd1d6d0badf04986602c943e287d44e46.tar.gz
	mv Halide-14.0.0-x86-64-osx ./Halide-Runtime
	rm Halide-14.0.0-x86-64-osx-6b9ed2afd1d6d0badf04986602c943e287d44e46.tar.gz
endif

.PHONY: setup-halide-runtime
ifeq ($(shell uname),Linux)
setup-halide-runtime: setup-halide-runtime_linux
	$(info setup-halide-runtime linux)
else
setup-halide-runtime: setup-halide-runtime_darwin
	$(info setup-halide-runtime darwin)
endif

.PHONY: build-generator
build-generator:
	docker build -f Dockerfile.generator -t $(_HALIDE):$(_HALIDE_VER) .
	docker tag $(_HALIDE):$(_HALIDE_VER) $(_HALIDE):latest

.PHONY: generate
generate: build-generator
	docker run --rm \
		-v $(PWD):/app -v $(PWD):/out $(_HALIDE):$(_HALIDE_VER)

.PHONY: build
build:
	docker build -t $(_NAME):$(_VERSION) .
	docker tag $(_NAME):$(_VERSION) $(_NAME):latest

.PHONY: ghpkg
ghpkg:
	docker tag $(_NAME):$(_VERSION) docker.pkg.github.com/octu0/blurry/$(_NAME):$(_VERSION)
	docker push docker.pkg.github.com/octu0/blurry/$(_NAME):$(_VERSION)

.PHONY: prune
prune:
	docker builder prune
