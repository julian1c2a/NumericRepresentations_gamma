# Tests configuration for NumericRepresentations
# =============================================================================

# Test configuration
test_timeout = get_option('test_timeout')
test_args = []

# Add common test arguments
if get_option('run_long_tests')
  test_args += ['--long-tests']
endif

# =============================================================================
# BASIC COMPILATION TEST (no external dependencies)
# =============================================================================

basic_test_exe = executable(
  'basic_compilation_test',
  'basic_compilation_test.cpp',
  include_directories: inc_dirs,
  cpp_args: compile_args,
  install: false
)

test('basic_compilation', basic_test_exe)

# =============================================================================
# TEST EXECUTABLES (with Catch2)
# =============================================================================

# Test dependencies
test_deps = [catch2_dep]  # Removed numeric_repr_lib since it's not a library target
test_include_dirs = inc_dirs
test_compile_args = []

# Compiler-specific test flags
if is_msvc
  test_compile_args += [
    '/D_CRT_SECURE_NO_WARNINGS',  # Disable MSVC security warnings for tests
  ]
endif

# Define all test targets
test_targets = [
  # Main test suite (numbered tests 01-10)
  {
    'name': 'test_01_math_tables',
    'source': 'test_01_math_tables.cpp',
    'priority': 'high'
  },
  # {
  #   'name': 'test_02_append',
  #   'source': 'test_02_append.cpp',
  #   'priority': 'high'
  # },
  {
    'name': 'test_03_core_internal',
    'source': 'test_03_core_internal.cpp',
    'priority': 'high'
  },
  {
    'name': 'test_04_dig_t',
    'source': 'test_04_dig_t.cpp',
    'priority': 'high'
  },
  {
    'name': 'test_05_dig_t_constructors',
    'source': 'test_05_dig_t_constructors.cpp',
    'priority': 'high'
  },
  {
    'name': 'test_06_dig_t_assignations',
    'source': 'test_06_dig_t_assignations.cpp',
    'priority': 'high'
  },
  {
    'name': 'test_07_dig_t_conversions',
    'source': 'test_07_dig_t_conversions.cpp',
    'priority': 'high'
  },
  {
    'name': 'test_08_dig_t_operadores',
    'source': 'test_08_dig_t_operadores.cpp',
    'priority': 'high'
  },
  {
    'name': 'test_09_dig_t_algebra',
    'source': 'test_09_dig_t_algebra.cpp',
    'priority': 'high'
  },
  {
    'name': 'test_10_dig_t_io',
    'source': 'test_10_dig_t_io.cpp',
    'priority': 'high'
  },
  # Other tests
  {
    'name': 'test_LUT_of_primes',
    'source': 'test_LUT_of_primes.cpp',
    'priority': 'medium'
  },
  {
    'name': 'test_primes_compiletime_catch2',
    'source': 'test_primes_compiletime_catch2.cpp',
    'priority': 'medium'
  },
]

# =============================================================================
# CREATE TEST EXECUTABLES
# =============================================================================

test_exes = {}

foreach test_info : test_targets
  test_name = test_info.get('name')
  test_source = test_info.get('source')
  test_priority = test_info.get('priority', 'medium')
  
  # Create executable
  test_exe = executable(
    test_name,
    test_source,
    dependencies: test_deps,
    include_directories: test_include_dirs,
    cpp_args: test_compile_args,
    install: false
  )
  
  # Store reference
  test_exes += {test_name: test_exe}
  
  # Register test with appropriate timeout
  test_timeout_value = test_timeout
  if test_priority == 'high'
    test_timeout_value = test_timeout * 2  # More time for critical tests
  endif
  
  test(
    test_name,
    test_exe,
    args: test_args,
    timeout: test_timeout_value,
    suite: [test_priority, 'all']
  )
endforeach

# =============================================================================
# TEST SUMMARY
# =============================================================================

if test_exes.keys().length() > 0
  message('Configured @0@ test executables'.format(test_exes.keys().length()))
  
  # List all configured tests
  test_list = []
  foreach test_name, test_exe : test_exes
    test_list += test_name
  endforeach
  
  message('Tests: ' + ', '.join(test_list))
endif