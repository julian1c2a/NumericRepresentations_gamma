#!/usr/bin/env python3
"""
Script to extract Catch2 paths from Conan cache for Meson integration
"""
import subprocess
import json
import os
import sys

def get_catch2_paths():
    """Get Catch2 include and lib paths from Conan"""
    try:
        # Get Conan package info
        result = subprocess.run(
            ['conan', 'cache', 'path', 'catch2/3.5.0'], 
            capture_output=True, text=True, check=True
        )
        base_path = result.stdout.strip()
        
        # Standard Conan package structure
        include_path = os.path.join(base_path, 'include').replace('\\', '/')
        lib_path = os.path.join(base_path, 'lib').replace('\\', '/')
        
        # Check if paths exist
        if os.path.exists(include_path) and os.path.exists(lib_path):
            return include_path, lib_path
        
        # Fallback: search in common locations
        for subdir in ['', 'build', 'install', 'package']:
            test_include = os.path.join(base_path, subdir, 'include')
            test_lib = os.path.join(base_path, subdir, 'lib')
            if os.path.exists(test_include) and os.path.exists(test_lib):
                return test_include.replace('\\', '/'), test_lib.replace('\\', '/')
                
    except Exception as e:
        print(f"Error detecting Catch2: {e}", file=sys.stderr)
    
    return None, None

def main():
    include_dir, lib_dir = get_catch2_paths()
    
    if include_dir and lib_dir:
        print(f"CATCH2_INCLUDE={include_dir}")
        print(f"CATCH2_LIB={lib_dir}")
        return 0
    else:
        print("CATCH2_NOT_FOUND=1")
        return 1

if __name__ == '__main__':
    sys.exit(main())