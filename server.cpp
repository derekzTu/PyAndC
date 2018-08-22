#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <stdarg.h>

// opencv
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

#define PORT 2000
#define YOLO_DIM 416
#define YOLO_RECIEVE 6
#define HALF_SECOND 50000
typedef unsigned char byte;

int main(int argc, char const *argv[]) {

    struct sockaddr_in address;
    struct sockaddr_in serverAddress;
	int sock = 0;
    char const *status = "Ready";

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    memset(&serverAddress, '0', sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to python
    if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    int retBytes = 0;
    int imgSize = 0;
    char size[YOLO_RECIEVE];
    byte* recievedBytes;
    int count = 0;
    while (true) {
        cv::Mat img = cv::Mat::zeros(YOLO_DIM, YOLO_DIM, CV_8UC1);
        // Tell python that we're ready to recieve next image
        if (retBytes = send(sock , status , strlen(status) , 0 )==-1) {
            printf("Error, cannot send status\n");
            break;
        }

        // Get image size
        if (retBytes = recv(sock, size, YOLO_RECIEVE, 0)==-1) {
            printf("Error cannot recieve image size\n");
            break;
        }

        imgSize = atoi(size);
        recievedBytes = new byte[imgSize];
        // Get image data and make it into readable format for showing on cpp side
        cv::Mat rawImage = cv::Mat::zeros(1, imgSize, CV_8UC1);
        if (retBytes =recv(sock, recievedBytes, imgSize, 0)==-1) {
            printf("Error cannot recieve image\n");
            break;
        }
        //img = cv::imdecode(rawImage, CV_LOAD_IMAGE_COLOR);
        if (count == 0) {
            printf("Byte value: %x\n", recievedBytes[0]);
        }
        count++;
        // Show image
        //cv::imshow("Python Image", img);
    	//cv::waitKey(5);
		usleep(HALF_SECOND);

    }
    // Finished doing stuff, tell server that we're done
    delete(recievedBytes);
    cv::destroyAllWindows();
    status = "Done";
    printf("Finished grabbing data, ending connection. \n");
    send(sock, status, strlen(status),0);
    close(sock);
    return 0;
}