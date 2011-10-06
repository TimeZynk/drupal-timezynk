#!/usr/bin/env ruby

workdir = File.basename(Dir.getwd)
function_name = "__#{workdir}_autoload"

out = File.open("#{workdir}.autoload.inc", "w");
out.puts "<?php"
out.puts "function #{function_name}($class_name) {"
out.puts "  switch ($class_name) {"
Dir.glob('**/*.class.inc') do |filename|
  File.open(filename, 'r') do |f|
    found_class = false
    while line = f.gets do
      /^(abstract class|class|interface) (\w+)/.match(line) do |m|
        found_class = true
        out.puts "    case '#{m[2]}':"
      end
    end
    if found_class
        out.puts "      require dirname(__FILE__) . '/#{filename}';"
        out.puts "      return TRUE;"
    end
  end
end
out.puts "    default:"
out.puts "      return FALSE;"
out.puts "  }"
out.puts "}"
out.puts "spl_autoload_register('#{function_name}', TRUE, TRUE);"
