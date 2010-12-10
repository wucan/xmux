#!/bin/sh

git archive --format=tar --prefix=xmux-latest/ HEAD | gzip > xmux-latest.tar.gz

