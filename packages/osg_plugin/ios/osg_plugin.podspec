#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html.
# Run `pod lib lint osg_plugin.podspec` to validate before publishing.
#
Pod::Spec.new do |s|
  s.name             = 'osg_plugin'
  s.version          = '0.0.1'
  s.summary          = 'A Flutter plugin for OSG rendering.'
  s.description      = <<-DESC
A Flutter plugin project for OpenSceneGraph rendering.
                       DESC
  s.homepage         = 'http://example.com'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'Your Company' => 'email@example.com' }
  s.source           = { :path => '.' }
  s.source_files     = 'Classes/**/*'
  s.dependency 'Flutter'

  s.platform = :ios, '12.0'
  s.pod_target_xcconfig = { 
    'DEFINES_MODULE' => 'YES',
    'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'i386'
  }
  s.swift_version = '5.0'
  
  s.public_header_files = 'Classes/OsgBridge.h'
  s.private_header_files = 'Classes/OsgRenderer.h'
end
