# SPDX-FileCopyrightText: 2024 Friedrich W. H. Kossebau <kossebau@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

function(install_theme name)
    cmake_parse_arguments(ARG "" "SVG" "" ${ARGN})
    if (NOT ARG_SVG)
        set(ARG_SVG "${name}.svg")
    endif()
    set(svgz "${CMAKE_CURRENT_BINARY_DIR}/${ARG_SVG}z")
    generate_svgz(${ARG_SVG} ${svgz} "theme-")
    install(
        FILES
            ${svgz}
        DESTINATION ${KDE_INSTALL_DATADIR}/katomic/themes
    )
endfunction()

# default theme is installed without .desktop file since KGameTheme does not need it
# when only one theme is available. See: 30a1cadd20
install_theme(default)
