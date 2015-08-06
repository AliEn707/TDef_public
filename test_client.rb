require 'socket'      # Sockets are in standard library

#puts "hostname"
hostname = "localhost"#gets.chomp
#puts "port"
port = 7000#gets.to_i

s = TCPSocket.open(hostname, port)

#puts connected
s.write [3].pack("c")
s.write [1].pack("c")
s.write [1].pack("l")
loop do
 a=s.read(4).unpack("l")
break if (a[0]==0)
puts s.read(a[0])
end
#while line = s.gets   # Read lines from the socket
  #puts line.chop      # And print with platform line terminator
#end
s.close               # Close the socket when done