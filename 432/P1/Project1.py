from socket import *
import sys
if len(sys.argv) <= 1:
    print('Usage : "python ProxyServer.py server_ip"\n[server_ip : It is the IP Address Of Proxy Server')
    sys.exit(2)
# Create a server socket, bind it to a port and start listening
tcpSerSock = socket(AF_INET, SOCK_STREAM)
# Fill in start.
SERVER_HOST = "127.0.0.1"
SERVER_PORT = 8000
tcpSerSock.bind((SERVER_HOST, SERVER_PORT))
tcpSerSock.listen(1)
# Fill in end.

while 1:
    # Start receiving data from the client
    print('Ready to serve...')
    tcpCliSock, addr = tcpSerSock.accept()
    print('Received a connection from:', addr)
    # Receive from client socket - decode to ensure bytes can be parsed as a string
    message = tcpCliSock.recv(2048).decode()
    print(message)
    # Extract the filename from the given message
    print(message.split()[1])
    filename = message.split()[1].partition("/")[2]
    print(filename)
    fileExist = "false"
    filetouse = "/" + filename
    print(filetouse)
    try:

        # Check whether the file exist in the cache
        f = open(filetouse[1:], "r")
        outputdata = f.readlines()
        fileExist = "true"
        # ProxyServer finds a cache hit and generates a response message
        tcpCliSock.send(b"HTTP/1.0 200 OK\r\n")
        tcpCliSock.send(b"Content-Type:text/html\r\n")
        # Parse site data, encoding as bytes prior to sending to client
        output = ''
        for line in outputdata:
            output += line
        tcpCliSock.sendall(output.encode())
        f.close()
        print('Read from cache')
        # Error handling for file not found in cache

    except IOError:
        if fileExist == "false":
            # Create a socket on the proxyserver
            c = socket(AF_INET, SOCK_STREAM)
            hostn = filename.replace("www.","",1)
            try:
                # Connect to the socket to port 80
                # Using hostname, for automatic IP resolution
                hostn = hostn.strip('\n')
                hostn = hostn.strip('\r')
                c.connect((hostn,80))
                # Create a temporary file on this socket and ask port 80 for the file requested by the client
                fileobj = c.makefile('wrb', 0)
                fileobj.write(("GET  " + "http://" + filename + " HTTP/1.0\n\n").encode())
                # Read the response into buffer
                # Parse the fileobj (data from the site), reading each line, decoding from byte to string
                buf =''
                while 1: 
                    line = fileobj.readline().decode()
                    if not line:
                        break
                    buf += line
                # Create a new file in the cache for the requested file.
                # Also send the response in the buffer to client socket and the corresponding file in the cache
                tmpFile = open("./" + filename,"wb")
                # Encode back to bytes, then output to cache file and send to client
                tmpFile.write(buf.encode())
                tmpFile.close()
                tcpCliSock.sendall(buf.encode())
                fileobj.close()
                c.close()
            except Exception as e:
                # Error recognition unsolved.. 
                print("Illegal request")
                print("exception E: ",e)

        else:
            # HTTP response message for file not found
            # Generic, though as stated above, error recognition was not solved..
            print("404 - page not found")
    # Close the client and the server sockets
    tcpCliSock.close()
# Close the server socket..
tcpSerSock.close()
            