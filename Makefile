build:
	@echo "Building tex"
	@docker run --rm \
		-v $(PWD)/02-rapport:/tex \
		chaussebenjamin/tex-compiler:latest \
		00-main.tex