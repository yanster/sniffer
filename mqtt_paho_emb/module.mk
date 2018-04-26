pwd := $(brazil.pwd)

include $(BrazilMake.dir)/targets/copy.mk

include CopySrc.mk

.PHONY: clean
clean:
	rm -rf $(brazil.build.dir)/*
