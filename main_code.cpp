#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

Mat image;
struct Template{
    char id;
    int img[50][50];
} ;

//character recognition function
void char_recognition(Mat ch)
{
    char rec1,rec2;
    char filename[6];
    Template x;
    int count,max1=0,max2=0,t=0;
    for (int i = 0; i < 62; ++i)
    {
        if(i<10){
            filename[0]=i+'0';
        }
        else if(i<36){
            filename[0]=i-10+'A';
        }
        else{
            filename[0]=i-36+'a';
        }
        filename[1]='\0';
        strcat(filename,".dat");
        ifstream infile;
        infile.open(filename,ios::in|ios::binary);
        infile.seekg(0,ios::beg);
        infile.read((char *)&x,sizeof(x));
        count=0;
        for (int j = 0; j < 50; ++j)
        {
            for (int k = 0; k < 50; ++k)
            {
                if((int(ch.at<uchar>(j,k))==255&&x.img[j][k]==1)||(int(ch.at<uchar>(j,k))==x.img[j][k])){
                    count++;
                }
            }
        }
        if(count>max1){
            rec2=rec1;
            rec1=x.id;
            max2=max1;
            max1=count;
        }
    }
    cout<<"\n"<<rec1<<"   "<<max1<<"    "<<rec2<<"    "<<max2;
}

//function to resize image to 50*50 pixels
void character_resize(int top,int bottom,int left,int right)
{
    Rect roi;
    roi.x = left;
    roi.y = top;
    roi.width = right - left + 1;
    roi.height = bottom - top + 1;
    Mat char_img=image(roi);
    resize(char_img,char_img,Size(50,50));
    char_recognition(char_img);
}

int main( int argc, char** argv )
{
    //image argument present
    if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }

    image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);   // Read the file

    cv::bitwise_not(image,image);                       //negative file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }


    //convert to pure black n white image
    for(int i=0;i<image.rows;i++){
        for (int j = 0; j < image.cols; ++j)
        {
            if(image.at<uchar>(i,j)>=128){
                image.at<uchar>(i,j)=255;
            }
            else image.at<uchar>(i,j)=0;
        }
    }


    //find top and bottom coordinates of each text line
    int top[1000]={0},bottom[1000]={0};
    int flag=0,count=0,k=0,i,j;

    for(i=0;i<image.rows;i++){
        for(j=0;j<image.cols;j++){
            if(image.at<uchar>(i,j)==255 && flag==0){
                flag=1;
                top[k]=i;
                count++;
                break;
            }
            if(image.at<uchar>(i,j)==255){
                count++;
            }
        }
        if(flag==1 && count==0){
            bottom[k++]=i;
            flag=0;
        }
        count=0;
    }
    if(flag==1&&--i==image.rows-1){
    bottom[k++]=i;
    }
    /*
    cout<<"k="<<k<<endl;
    for(int i=0;i<k;i++){
        cout<<i<<"       "<<top[i]<<"   "<<bottom[i]<<"\n";
    }
    */

    //find left and right boundary for each character
    int left=0,right=0;
    int x,y;
    flag=0;
    for(i=0;i<k;i++){
        for(x=0;x<image.cols;x++){
            for(int y=top[i];y<=bottom[i];y++){
                if(flag==0 && image.at<uchar>(y,x)==255){
                    flag=1;
                    left=x;
                    count++;
                    break;
                }
                if(flag==1 && image.at<uchar>(y,x)==255){
                count++;
                }
            }
            if(flag==1 && count==0){
            right=x;
            flag=0;
            character_resize(top[i],bottom[i],left,right);
            }
        count=0;
        }
    }
    if(flag==1 && image.cols-1==--x){
        right=x;
        character_resize(top[i],bottom[i],left,right);
    }

    /*for (int i = 0; i < image.rows; ++i)
    {
        for (int j = 0; j < image.cols; ++j)
        {
            cout<<(int(image.at<uchar>(i,j))==255?1:0);
        }
        cout<<endl;
    }*/


    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", image );                   // Show our image inside it.

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}
