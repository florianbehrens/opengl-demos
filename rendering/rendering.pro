TEMPLATE = subdirs
SUBDIRS = ex_instanced_geometry \
          ex_instanced_histogram \
          ex_wireframe

!mac {
    SUBDIRS += ex_quad_tessellation \
               ex_terrain_tessellation \
               ex_terrain_tessellation2
}
