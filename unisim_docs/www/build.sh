#!/bin/bash

DEFAULT_THEME=softy

rm -rf site
mkdir -p site
mkdir -p site/images

THEMES=`cd themes; ls --color=never`

for THEME in ${THEMES}; do
	if [ -d "themes/${THEME}" ]; then
		THEME_NAV=
		for THEME2 in ${THEMES}; do
			if [ "${THEME}" = "${DEFAULT_THEME}" ]; then
				if [ "${THEME2}" = "${DEFAULT_THEME}" ]; then
					THEME_NAV="${THEME_NAV} <a href=\"index.html\">${THEME2}</a>"
				else
					THEME_NAV="${THEME_NAV} <a href=\"${THEME2}/index.html\">${THEME2}</a>"
				fi
			else
				if [ "${THEME2}" = "${DEFAULT_THEME}" ]; then
					THEME_NAV="${THEME_NAV} <a href=\"../index.html\">${THEME2}</a>"
				else
					THEME_NAV="${THEME_NAV} <a href=\"../${THEME2}/index.html\">${THEME2}</a>"
				fi
			fi
		done

		if [ "${THEME}" = "${DEFAULT_THEME}" ]; then
			ROOT=
		else
			ROOT="${THEME}"
			mkdir -p site/${THEME}
		fi

		mkdir -p site/${ROOT}/theme
		mkdir -p site/${ROOT}/images

		echo "Building stylesheet for theme ${THEME}"
		cpp -P -Itemplate -Ithemes/${THEME} template/template.css > site/${ROOT}/theme/style.css || exit 1
		cp themes/${THEME}/*.png site/${ROOT}/theme


		CONTENTS=`cd content; find . -name "*.html"`

		for CONTENT in ${CONTENTS}; do
			CONTENT_DIR=`dirname ${CONTENT}`
			echo "Building ${CONTENT_DIR}.html for theme ${THEME}..."
			cpp "-DTHEME_NAV=${THEME_NAV}" -P -Itemplate -Icontent/${CONTENT_DIR} template/template.html > site/${ROOT}/${CONTENT_DIR}.html || exit -1
		done

		IMAGES=`cd images; find . -name "*.png"`

		for IMAGE in ${IMAGES}; do
			if [ -f "images/${IMAGE}" ]; then
				echo "Copying images/${IMAGE} for theme ${THEME}"
				mkdir -p site/${THEME}/images/`dirname "${IMAGE}"`
				cp "images/${IMAGE}" "site/${ROOT}/images/${IMAGE}"
			fi
		done
	fi
done


# CONTENTS=`cd content; find . -name "*.html"`
# 
# for CONTENT in ${CONTENTS}; do
# 	CONTENT_DIR=`dirname ${CONTENT}`
# 	echo "Building ${CONTENT_DIR}.html..."
# 	cpp -DSTYLESHEET=\"themes/${STYLE}/style.css\" -P -Itemplate -Icontent/${CONTENT_DIR} template/template.html > site/${CONTENT_DIR}.html || exit -1
# done

echo "The web site is in ${PWD}/site"
