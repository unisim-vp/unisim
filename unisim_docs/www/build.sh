#!/bin/bash

DEFAULT_THEME=softy

rm -rf site
mkdir -p site
mkdir -p site/images
mkdir -p site/downloads

THEMES=`cd themes; ls --color=never`

for THEME in ${THEMES}; do
	if [ -d "themes/${THEME}" ]; then
		if [ "${THEME}" = "${DEFAULT_THEME}" ]; then
			THEME_ROOT=
		else
			THEME_ROOT="${THEME}"
			mkdir -p site/${THEME}
		fi

		mkdir -p site/${THEME_ROOT}/style

		echo "Building stylesheet for theme ${THEME}"
		cpp -P -Itemplate -Ithemes/${THEME} template/template.css > site/${THEME_ROOT}/style/style.css || exit 1
		cp themes/${THEME}/*.png site/${THEME_ROOT}/style


		CONTENTS=`cd content; ls */*.html`

		for CONTENT in ${CONTENTS}; do
			CONTENT_DIR=`dirname ${CONTENT}`
			echo "Building ${CONTENT_DIR}.html for theme ${THEME}..."

			THEME_NAV=
			for THEME2 in ${THEMES}; do
				if [ "${THEME}" = "${DEFAULT_THEME}" ]; then
					if [ "${THEME2}" = "${DEFAULT_THEME}" ]; then
						THEME_NAV="${THEME_NAV} <a href=\"${CONTENT_DIR}.html\">${THEME2}</a>"
					else
						THEME_NAV="${THEME_NAV} <a href=\"${THEME2}/${CONTENT_DIR}.html\">${THEME2}</a>"
					fi
				else
					if [ "${THEME2}" = "${DEFAULT_THEME}" ]; then
						THEME_NAV="${THEME_NAV} <a href=\"../${CONTENT_DIR}.html\">${THEME2}</a>"
					else
						THEME_NAV="${THEME_NAV} <a href=\"../${THEME2}/${CONTENT_DIR}.html\">${THEME2}</a>"
					fi
				fi
			done

			if [ "${THEME}" = "${DEFAULT_THEME}" ]; then
				SITE_PREFIX=
			else
				SITE_PREFIX="../"
			fi

			cpp "-DTHEME_NAV=${THEME_NAV}" "-DIMAGES=${SITE_PREFIX}images" "-DDOWNLOADS=${SITE_PREFIX}downloads" -P -Itemplate -Icontent/${CONTENT_DIR} template/template.html > site/${THEME_ROOT}/${CONTENT_DIR}.html || exit -1
		done
	fi
done

IMAGES=`cd images; find . -name "*.png"`

for IMAGE in ${IMAGES}; do
	if [ -f "images/${IMAGE}" ]; then
		echo "Copying images/${IMAGE}"
		mkdir -p site/images/`dirname "${IMAGE}"`
		cp "images/${IMAGE}" "site/images/${IMAGE}"
	fi
done

echo "The web site is in ${PWD}/site"
