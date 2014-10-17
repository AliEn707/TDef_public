require 'socket'      # Sockets are in standard library

#puts "hostname"
hostname = "localhost"#gets.chomp
#puts "port"
port = 7001#gets.to_i

s = TCPSocket.open(hostname, port)

#puts connected
s.puts "c1"
#while line = s.gets   # Read lines from the socket
  #puts line.chop      # And print with platform line terminator
#end
s.close               # Close the socket when done