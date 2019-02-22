# Teapot v3.0.0 configuration generated at 2019-02-23 11:41:50 +1300

required_version "3.0"

define_project "parallel" do |project|
	project.title = "Parallel"
end

# Build Targets

define_target 'parallel-library' do |target|
	target.depends 'Language/C++14'
	
	target.provides 'Library/Parallel' do
		source_root = target.package.path + 'source'
		
		# library_path = build static_library: 'Parallel', source_files: source_root.glob('Parallel/**/*.cpp')
		# 
		# append linkflags library_path
		
		append header_search_paths source_root
	end
end

define_target 'parallel-test' do |target|
	target.depends 'Library/Parallel'
	target.depends 'Library/UnitTest'
	
	target.depends 'Language/C++14'
	
	target.provides 'Test/Parallel' do |arguments|
		test_root = target.package.path + 'test'
		
		run tests: 'Parallel', source_files: test_root.glob('Parallel/**/*.cpp'), arguments: arguments
	end
end

# Configurations

define_configuration 'development' do |configuration|
	configuration[:source] = "https://github.com/kurocha"
	configuration.import "parallel"
	
	# Provides all the build related infrastructure:
	configuration.require 'platforms'
	
	# Provides unit testing infrastructure and generators:
	configuration.require 'unit-test'
	
	# Provides some useful C++ generators:
	configuration.require 'generate-cpp-class'
	
	configuration.require "generate-project"
end

define_configuration "parallel" do |configuration|
	configuration.public!
end
