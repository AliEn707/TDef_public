require 'socket'      # Sockets are in standard library

#puts "hostname"
hostname = "localhost"#gets.chomp
#puts "port"
port = 7001#gets.to_i

s = TCPSocket.open(hostname, port)

#puts connected
s.write [4].pack("l")
s.puts "1234567890123"
s.puts "test"
s.puts "12345678901234"
s.puts "Lc"
s.puts "Lc"
p s.read.unpack("l")
#while line = s.gets   # Read lines from the socket
  #puts line.chop      # And print with platform line terminator
#end
s.close               # Close the socket when done