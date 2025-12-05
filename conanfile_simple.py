from conan import ConanFile

class NumericRepresentationsConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    
    def requirements(self):
        self.requires("catch2/3.11.0")
        
    def generate(self):
        pass