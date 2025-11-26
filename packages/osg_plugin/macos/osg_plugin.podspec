#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html.
# Run `pod lib lint osg_plugin.podspec` to validate before publishing.
#
Pod::Spec.new do |s|
  s.name             = 'osg_plugin'
  s.version          = '0.0.1'
  s.summary          = 'A new Flutter plugin project.'
  s.description      = <<-DESC
A new Flutter plugin project.
                       DESC
  s.homepage         = 'http://example.com'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'Your Company' => 'email@example.com' }
  s.source           = { :path => '.' }
  s.source_files     = 'Classes/**/*'
  s.dependency 'FlutterMacOS'

  s.platform = :osx, '10.11'
  s.pod_target_xcconfig = { 
    'DEFINES_MODULE' => 'YES'
  }
  s.swift_version = '5.0'
  
  s.public_header_files = 'Classes/OsgBridge.h'
  s.private_header_files = 'Classes/OsgRenderer.h'

  # OSG Configuration
  # Assuming Homebrew installation on Apple Silicon (/opt/homebrew) or Intel (/usr/local)
  s.xcconfig = {
    'HEADER_SEARCH_PATHS' => '/opt/homebrew/include /usr/local/include',
    'LIBRARY_SEARCH_PATHS' => '/opt/homebrew/lib /usr/local/lib',
    'OTHER_LDFLAGS' => '-losg -losgViewer -losgDB -losgGA -losgUtil -lOpenThreads'
  }
end
