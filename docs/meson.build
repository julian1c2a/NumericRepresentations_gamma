# Documentation generation with Doxygen
# =============================================================================

doxygen = find_program('doxygen', required: false)

if doxygen.found() and get_option('enable_docs')
  
  # Doxygen configuration
  doxy_conf = configuration_data()
  doxy_conf.set('PROJECT_NAME', 'NumericRepresentations')
  doxy_conf.set('PROJECT_VERSION', meson.project_version())
  doxy_conf.set('PROJECT_BRIEF', 'Advanced numeric type system with unlimited precision')
  doxy_conf.set('INPUT_DIR', join_paths(meson.source_root(), 'include'))
  doxy_conf.set('OUTPUT_DIR', meson.current_build_dir())
  doxy_conf.set('EXAMPLE_PATH', join_paths(meson.source_root(), 'examples'))
  
  # Configure Doxyfile
  doxyfile = configure_file(
    input: 'Doxyfile.in',
    output: 'Doxyfile',
    configuration: doxy_conf
  )
  
  # Documentation target
  docs = custom_target(
    'docs',
    command: [doxygen, doxyfile],
    output: ['html'],
    build_by_default: false,
    install: false
  )
  
  message('Documentation generation enabled (run: meson compile docs)')
  
else
  warning('Doxygen not found or documentation disabled')
endif