#!/bin/bash
rm -rf site
mkdir -p site
mkdir -p site/images
mkdir -p site/themes

THEMES=`cd themes; find . -name "*.css"`

for THEME in ${THEMES}; do
	if [ -f "themes/${THEME}" ]; then
		echo "Copying ${THEME}"
		mkdir -p site/themes/`dirname "${THEME}"`
		cp "themes/${THEME}" "site/themes/${THEME}"
	fi
done

IMAGES=`cd images; find . -name "*.png"`

for IMAGE in ${IMAGES}; do
	if [ -f "images/${IMAGE}" ]; then
		echo "Copying ${IMAGE}"
		mkdir -p site/images/`dirname "${IMAGE}"`
		cp "images/${IMAGE}" "site/images/${IMAGE}"
	fi
done

CONTENTS=`cd content; find . -name "*.html"`

for CONTENT in ${CONTENTS}; do
	CONTENT_DIR=`dirname ${CONTENT}`
	echo "Building ${CONTENT_DIR}.html..."
	cpp -P -Itemplate -Icontent/${CONTENT_DIR} template/template.html > site/${CONTENT_DIR}.html || exit -1
done

echo "The web site is in ${PWD}/site"
