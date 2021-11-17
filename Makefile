VERSION_GO  = version.go

_NAME       = $(shell grep -o 'AppName string = "[^"]*"' $(VERSION_GO)  | cut -d '"' -f2)
_VERSION    = $(shell grep -oE 'Version string = "[0-9]+\.[0-9]+\.[0-9]+"' $(VERSION_GO) | cut -d '"' -f2)

_HALIDE     = "generator"
_HALIDE_VER = "13.0.1"

.PHONY: vet
vet:
	go vet ./

.PHONY: test
test:
	go test -v ./...

ifeq ($(shell uname),Darwin)
.PHONY: setup-halide
setup-halide:
	brew install halide

.PHONY: setup-halide-runtime
setup-halide-runtime:
ifeq ($(shell [ -d ./Halide-Runtime ] && echo "1"),1)
	@echo "./Halide-Runtime exists"
else
	curl -O -sSL https://github.com/halide/Halide/releases/download/v13.0.1/Halide-13.0.1-x86-64-osx-fb39d7e3149c0ee1e848bb9957be2ea18765c35d.tar.gz
	tar xzf Halide-13.0.1-x86-64-osx-fb39d7e3149c0ee1e848bb9957be2ea18765c35d.tar.gz
	mv Halide-13.0.1-x86-64-osx ./Halide-Runtime
	rm Halide-13.0.1-x86-64-osx-fb39d7e3149c0ee1e848bb9957be2ea18765c35d.tar.gz
endif
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
