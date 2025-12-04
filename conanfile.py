from conan import ConanFile
from conan.tools.meson import MesonToolchain
from conan.tools.layout import basic_layout
from conan.tools.files import copy
import os


class NumericRepresentationsConan(ConanFile):
    name = "numeric-representations"
    version = "1.0.0"
    
    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "fPIC": [True, False],
        "enable_tests": [True, False],
        "enable_docs": [True, False],
        "with_benchmarks": [True, False],
    }
    default_options = {
        "fPIC": True,
        "enable_tests": True,
        "enable_docs": True,
        "with_benchmarks": False,
    }
    
    # Sources are located in the same place as this recipe
    exports_sources = "include/*", "tests/*", "src/*", "meson.build", "meson.options"
    
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
            
    def requirements(self):
        # Main dependencies
        if self.options.enable_tests:
            self.requires("catch2/3.5.0")
            
    def build_requirements(self):
        # Build tools
        self.tool_requires("meson/1.3.0")
        
    def layout(self):
        basic_layout(self, src_folder="src")
        
    def generate(self):
        # Generate Meson toolchain with compiler settings
        tc = MesonToolchain(self)
        
        # Add custom Meson options based on Conan options
        tc.project_options["enable_tests"] = str(self.options.enable_tests).lower()
        tc.project_options["enable_docs"] = str(self.options.enable_docs).lower()
        tc.project_options["enable_benchmarks"] = str(self.options.with_benchmarks).lower()
        
        # Compiler-specific optimizations
        if self.settings.build_type == "Release":
            tc.project_options["native_optimizations"] = "true"
            
        # MSVC-specific settings
        if str(self.settings.compiler) in ["Visual Studio", "msvc"]:
            tc.project_options["deep_constexpr"] = "true"
            
        tc.generate()
        
    def package_info(self):
        # Header-only library
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
        
        # Include directories
        self.cpp_info.includedirs = ["include"]
        
        # Compiler-specific definitions and flags
        if str(self.settings.compiler) in ["Visual Studio", "msvc"]:
            self.cpp_info.cppflags.extend([
                "/constexpr:depth2048",
                "/constexpr:steps1048576", 
                "/bigobj",
                "/permissive-"
            ])
        elif str(self.settings.compiler) in ["gcc", "clang"]:
            self.cpp_info.cppflags.extend([
                "-fconstexpr-depth=2048"
            ])
            if str(self.settings.compiler) == "clang":
                self.cpp_info.cppflags.append("-fconstexpr-steps=1048576")
            else:  # gcc
                self.cpp_info.cppflags.append("-fconstexpr-ops-limit=1048576")
                
        # Required C++ standard
        if hasattr(self.cpp_info, 'cppstd'):
            self.cpp_info.cppstd = "23"

# En lugar de: test_executables.length()
# Usar: test_executables.keys().length()
# O simplemente iterar sobre las claves

foreach test_name, test_exe : test_executables
    test(test_name, test_exe)
endforeach