#!/usr/bin/env python
# -*- coding: utf-8 -*-

import io
import socket
import struct
import time
import sys
import cv2
import numpy

def main():
	# Set up the socket on the python side
	self_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	self_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	self_socket.bind(("localhost", 2000))
	self_socket.listen(5)

	# Accept cpp side hail
	client_socket, address = self_socket.accept()

	cap = cv2.VideoCapture(0)
	count = 0
	# Send stuff to socket until told otherwise
	encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), 90]
	while 1:
		status = client_socket.recv(1024).decode()
		if status == "Ready" :
			ret, frame = cap.read()

			# Process data for YOLO
			resized_frame = cv2.resize(frame, (416, 416))

			# Encode data to send
			result, encodedFrame = cv2.imencode('.jpg', resized_frame, encode_param)
			data = numpy.array(encodedFrame)
			stringData = data.tostring()


			# Send size of image
			height, width = resized_frame.shape[:2]
			imgSize = height*width
			sendSize = str(imgSize)
			if count == 0:
				print(stringData)
			# Send data
			client_socket.send(sendSize.encode())
			client_socket.send(stringData);
			count+=1
		if status == "Done":
			break

	# Close the sockets
	self_socket.close()
	client_socket.close()
	cap.release()

if __name__ == '__main__':
   main()