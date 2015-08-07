require 'socket'      # Sockets are in standard library

#puts "hostname"
hostname = "localhost"#gets.chomp
#puts "port"
port = 7000#gets.to_i

s = TCPSocket.open(hostname, port)

#puts connected
s.write [3].pack("c")
s.write [4].pack("c")
#s.write [1].pack("l")
loop do
	p a=s.read(4).unpack("l")
	break if (a[0]==0)
	puts s.read(a[0])
	 s.write [1].pack("l")
	loop do
		puts "file"
		p a=s.read(4).unpack("l")
		break if (a[0]==0)
		puts s.read(a[0])
	end
end
#while line = s.gets   # Read lines from the socket
  #puts line.chop      # And print with platform line terminator
#end
s.close               # Close the socket when done