project(
  'NumericRepresentations',
  'cpp',
  version: '1.0.0',
  license: 'MIT',
  meson_version: '>=0.63.0',
  default_options: [
    'cpp_std=c++23',
    'buildtype=debugoptimized',
    'warning_level=3',
    'werror=false',
    'b_sanitize=none',
  ]
)

# =============================================================================
# COMPILER DETECTION AND FLAGS
# =============================================================================
cpp = meson.get_compiler('cpp')
compiler_sim = get_option('compiler_simulation')

# Real compiler detection
real_msvc = cpp.get_id() == 'msvc'
real_gcc = cpp.get_id() == 'gcc' 
real_clang = cpp.get_id() == 'clang'

# Simulated compiler behavior
if compiler_sim == 'auto'
  is_msvc = real_msvc
  is_gcc = real_gcc
  is_clang = real_clang
else
  is_msvc = compiler_sim == 'msvc'
  is_gcc = compiler_sim == 'gcc'
  is_clang = compiler_sim == 'clang'
endif

# Common include directories
inc_dirs = include_directories('include')

# =============================================================================
# COMPILER-SPECIFIC FLAGS
# =============================================================================
compile_args = []

if is_msvc
  # MSVC simulation using GCC-compatible flags
  constexpr_depth = get_option('constexpr_depth')
  constexpr_ops = get_option('constexpr_ops')
  compile_args += [
    '-Wall', '-Wextra', 
    '-Wno-unused-parameter',
    '-fconstexpr-depth=' + constexpr_depth.to_string(),
    '-fconstexpr-ops-limit=' + constexpr_ops.to_string(),
    # Simulate MSVC's stricter behavior
    '-Werror=return-type',      # Equivalent to MSVC error on missing return
    '-Wno-pedantic',            # MSVC is less pedantic than GCC's pedantic mode
  ]
  
  # MSVC optimization flags by build type (GCC equivalents)
  if get_option('buildtype') == 'release'
    compile_args += ['-O2', '-DNDEBUG']
  elif get_option('buildtype') == 'debug'
    compile_args += ['-O0', '-g3', '-D_DEBUG']
  else
    compile_args += ['-O1', '-g1']
  endif
  
elif is_gcc
  # GCC-specific flags
  constexpr_depth = get_option('constexpr_depth')
  constexpr_ops = get_option('constexpr_ops')
  compile_args += [
    '-Wall', '-Wextra', '-Wpedantic',
    '-Wno-unused-parameter',
    '-fconstexpr-depth=' + constexpr_depth.to_string(),
    '-fconstexpr-ops-limit=' + constexpr_ops.to_string(),
  ]
  
  # GCC optimization by build type
  if get_option('buildtype') == 'release'
    compile_args += ['-O3', '-DNDEBUG', '-march=native']
  elif get_option('buildtype') == 'debug'
    compile_args += ['-O0', '-g3', '-fno-omit-frame-pointer']
  else
    compile_args += ['-O2', '-g1']
  endif
  
elif is_clang
  # Clang-specific flags (using GCC-compatible alternatives for simulation)
  constexpr_depth = get_option('constexpr_depth')
  constexpr_ops = get_option('constexpr_ops')
  compile_args += [
    '-Wall', '-Wextra', '-Wpedantic',
    '-Wno-unused-parameter',
    '-fconstexpr-depth=' + constexpr_depth.to_string(),
    '-fconstexpr-ops-limit=' + constexpr_ops.to_string(),  # Use GCC equivalent when simulating
    '-Wno-c++98-compat',  # Would be Clang-specific but ignored by GCC
  ]
  
  # Clang optimization by build type
  if get_option('buildtype') == 'release'
    compile_args += ['-O3', '-DNDEBUG', '-march=native']
  elif get_option('buildtype') == 'debug'
    compile_args += ['-O0', '-g', '-fno-omit-frame-pointer']
  else
    compile_args += ['-O2', '-g']
  endif
endif

# Add common compile arguments
add_project_arguments(compile_args, language: 'cpp')

# =============================================================================
# DEPENDENCIES
# =============================================================================

# Catch2 dependency detection with multiple fallback methods
catch2_dep = dependency('', required: false)
catch2_main_dep = dependency('', required: false)

# Method 1: Try standard dependency detection
if not catch2_dep.found()
  catch2_dep = dependency('catch2', required: false, method: 'auto')
endif

if not catch2_dep.found()
  catch2_dep = dependency('catch2-with-main', required: false, method: 'auto')
endif

# Method 2: Try CMake detection (for Conan packages)
if not catch2_dep.found()
  catch2_dep = dependency('Catch2', required: false, method: 'cmake')
endif

# Method 3: Manual library search
if not catch2_dep.found()
  catch2_lib = cpp.find_library('Catch2', required: false)
  catch2_main_lib = cpp.find_library('Catch2Main', required: false)
  
  if catch2_lib.found() and catch2_main_lib.found()
    catch2_dep = declare_dependency(
      dependencies: [catch2_lib, catch2_main_lib]
    )
  elif catch2_lib.found()
    catch2_dep = catch2_lib
  endif
endif

# Method 4: Conan-specific search using environment variables
if not catch2_dep.found()
  conan_catch2_include = get_option('conan_catch2_include_dir')
  conan_catch2_lib = get_option('conan_catch2_lib_dir')
  
  if conan_catch2_include != '' and conan_catch2_lib != ''
    catch2_dep = declare_dependency(
      include_directories: include_directories(conan_catch2_include),
      link_args: ['-L' + conan_catch2_lib, '-lCatch2', '-lCatch2Main']
    )
  endif
endif

# Final fallback - declare stub dependency
if not catch2_dep.found()
  warning('Catch2 not found via any method. Tests will be built without proper Catch2 linking.')
  warning('Please ensure Conan has installed Catch2 properly.')
  catch2_dep = declare_dependency()
endif

# =============================================================================
# PROJECT OPTIONS
# =============================================================================

# =============================================================================
# LIBRARY CONFIGURATION (Header-only)
# =============================================================================
numeric_repr_lib = declare_dependency(
  include_directories: inc_dirs,
  compile_args: compile_args
)

# =============================================================================
# SUBDIRECTORIES
# =============================================================================
if get_option('enable_tests')
  subdir('tests')
endif

if get_option('enable_docs')
  subdir('docs')
endif

# =============================================================================
# SUMMARY
# =============================================================================
summary(
  {
    'C++ Standard': get_option('cpp_std'),
    'Build Type': get_option('buildtype'),
    'Compiler': cpp.get_id() + ' ' + cpp.version(),
    'Platform': host_machine.system(),
    'Tests': get_option('enable_tests'),
    'Documentation': get_option('enable_docs'),
    'Benchmarks': get_option('enable_benchmarks'),
  },
  section: 'Configuration',
  bool_yn: true
)