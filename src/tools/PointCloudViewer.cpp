#include "tools/PointCloudViewer.h"

PointCloudViewer::PointCloudViewer(bool debugActive_)
{
    debugActive = debugActive_;

    reference_image_w = 640;
    reference_image_h = 480;

    image_w = reference_image_w;
    image_h = reference_image_h;

    reference_focal_distance = 300;
    setImageProperties(image_w, image_h, Scalar(0,0,0));

    //starting view
    original_camPos_x = 0;
    original_camPos_y = 0;
    original_camPos_z = -100;
    original_lookAt_x = 0;
    original_lookAt_y = 0;
    original_lookAt_z = 0;
    original_pitch = 0;
    original_yaw = 0;

    camPos_x = original_camPos_x;
    camPos_y = original_camPos_y;
    camPos_z = original_camPos_z;
    lookAt_x = original_lookAt_x;
    lookAt_y = original_lookAt_y;
    lookAt_z = original_lookAt_z;
    pitch = original_pitch;
    yaw = original_yaw;

    x_step = 10;
    y_step = 10;
    z_step = 10;
    yaw_step = 1;
    pitch_step = 1;

    TransfCam2Orig = TransformationFromCamToOrigin(camPos_x, camPos_y, camPos_z, pitch, yaw);
}

void PointCloudViewer::defineCameraParameters(double image_w, double image_h)
{
    optical_center_x = image_w/2;
    optical_center_y = image_h/2;

    double focal_distance_x = image_w*reference_focal_distance/reference_image_w;
    double focal_distance_h = image_h*reference_focal_distance/reference_image_h;

    K = Mat::eye(3,3,CV_64F);
    K.at<double>(0,0) = focal_distance_x;
    K.at<double>(1,1) = focal_distance_h;
    K.at<double>(0,2) = optical_center_x;
    K.at<double>(1,2) = optical_center_y;
}

void PointCloudViewer::setImageProperties(double image_w, double image_h, Scalar background_color_)
{
    defineCameraParameters(image_w, image_h);
    image = Mat(image_h, image_w, CV_8UC3);
    background_color = background_color_;
}

void PointCloudViewer::set(std::vector<cv::Point3f> pointCloudPoints, std::vector<cv::Point3f> pointCloudRGB)
{
    pointCloud = pointCloudPoints;
    pointCloudColor = pointCloudRGB;
}

void PointCloudViewer::view()
{
    {

        image = background_color;

        Parallel_PointCloudMapping parallel = Parallel_PointCloudMapping(pointCloud, pointCloudColor, TransfCam2Orig, K, image);
        cv::parallel_for_(cv::Range(0, pointCloud.size()), parallel);//*/

        //draw axis
        drawAxis(image, K, TransfCam2Orig, 2);

        //draw text
        if(debugActive)
            drawText(image);

        char key = waitKey(1);
        switch (key)
        {
            case 'q':
            {
                exit(0);
                break;
            }
            case 'r':
            {
                camPos_x = original_camPos_x;
                camPos_y = original_camPos_y;
                camPos_z = original_camPos_z;
                lookAt_x = original_lookAt_x;
                lookAt_y = original_lookAt_y;
                lookAt_z = original_lookAt_z;
                pitch = original_pitch;
                yaw = original_yaw;

                //TransfCam2Orig = TransformationFromCamToOrigin(camPos_x, camPos_y, camPos_z, lookAt_x, lookAt_y, lookAt_z);
                TransfCam2Orig = TransformationFromCamToOrigin(camPos_x, camPos_y, camPos_z, pitch, yaw);

                break;
            }

            case 'a':
            {
                camPos_x -= cos(pitch)*x_step;
                camPos_z -= -sin(pitch)*cos(yaw)*x_step;
                lookAt_x -= cos(pitch)*x_step;
                lookAt_z -= -sin(pitch)*cos(yaw)*x_step;

                TransfCam2Orig = TransformationFromCamToOrigin(camPos_x, camPos_y, camPos_z, pitch, yaw);

                break;
            }

            case 'd':
            {
                camPos_x += cos(pitch)*x_step;
                camPos_z += -sin(pitch)*cos(yaw)*x_step;
                lookAt_x += cos(pitch)*x_step;
                lookAt_z += -sin(pitch)*cos(yaw)*x_step;

                TransfCam2Orig = TransformationFromCamToOrigin(camPos_x, camPos_y, camPos_z, pitch, yaw);

                break;
            }

            case 'w':
            {
                camPos_x += -sin(pitch)*z_step;
                camPos_y += -sin(yaw)*z_step;
                camPos_z += cos(pitch)*cos(yaw)*z_step;
                lookAt_x += -sin(pitch)*z_step;
                lookAt_y += -sin(yaw)*z_step;
                lookAt_z += cos(pitch)*cos(yaw)*z_step;

                TransfCam2Orig = TransformationFromCamToOrigin(camPos_x, camPos_y, camPos_z, pitch, yaw);

                break;
            }

            case 's':
            {
                camPos_x -= -sin(pitch)*z_step;
                camPos_y -= -sin(yaw)*z_step;
                camPos_z -= cos(pitch)*cos(yaw)*z_step;
                lookAt_x -= -sin(pitch)*z_step;
                lookAt_y -= -sin(yaw)*z_step;
                lookAt_z -= cos(pitch)*cos(yaw)*z_step;

                TransfCam2Orig = TransformationFromCamToOrigin(camPos_x, camPos_y, camPos_z, pitch, yaw);

                break;
            }

            case 'y':
            {
                camPos_y -= cos(yaw)*y_step;
                camPos_z -= -cos(pitch)*sin(yaw)*y_step;
                lookAt_y -= cos(yaw)*y_step;
                lookAt_z -= -cos(pitch)*sin(yaw)*y_step;

                TransfCam2Orig = TransformationFromCamToOrigin(camPos_x, camPos_y, camPos_z, pitch, yaw);

                break;
            }

            case 'g':
            {
                camPos_y += cos(yaw)*y_step;
                camPos_z += -cos(pitch)*sin(yaw)*y_step;
                lookAt_y += cos(yaw)*y_step;
                lookAt_z += -cos(pitch)*sin(yaw)*y_step;

                TransfCam2Orig = TransformationFromCamToOrigin(camPos_x, camPos_y, camPos_z, pitch, yaw);

                break;
            }

            case 'i':
            {
                double y=(lookAt_y-camPos_y);
                double z=(lookAt_z-camPos_z);

                double vert_d = sqrt(y*y+z*z);

                yaw -= yaw_step*CV_PI/180.;
                if(yaw < -2*CV_PI)
                    yaw = 0;
                if(yaw > 2*CV_PI)
                    yaw = 0;

                y = -sin(yaw)*vert_d;
                z = sqrt(vert_d*vert_d - y*y);

                camPos_y = lookAt_y-y;
                camPos_z = lookAt_z-z;

                TransfCam2Orig = TransformationFromCamToOrigin(camPos_x, camPos_y, camPos_z, pitch, yaw);

                break;
            }

            case 'k':
            {
                double y=(lookAt_y-camPos_y);
                double z=(lookAt_z-camPos_z);

                double vert_d = sqrt(y*y+z*z);

                yaw += yaw_step*CV_PI/180.;
                if(yaw < -2*CV_PI)
                    yaw = 0;
                if(yaw > 2*CV_PI)
                    yaw = 0;

                y = -sin(yaw)*vert_d;
                z = sqrt(vert_d*vert_d - y*y);

                camPos_y = lookAt_y-y;
                camPos_z = lookAt_z-z;

                TransfCam2Orig = TransformationFromCamToOrigin(camPos_x, camPos_y, camPos_z, pitch, yaw);

                break;
            }

            case 'j':
            {
                double x=(lookAt_x-camPos_x);
                double z=(lookAt_z-camPos_z);

                double horiz_d = sqrt(x*x+z*z);

                pitch += pitch_step*CV_PI/180.;
                if(pitch < -2*CV_PI)
                    pitch = 0;
                if(pitch > 2*CV_PI)
                    pitch = 0;

                x = sin(pitch)*horiz_d;
                z = sqrt(horiz_d*horiz_d - x*x);

                camPos_x = lookAt_x-x;
                camPos_z = lookAt_z-z;

                TransfCam2Orig = TransformationFromCamToOrigin(camPos_x, camPos_y, camPos_z, pitch, yaw);

                break;
            }

            case 'l':
            {
                double x=(lookAt_x-camPos_x);
                double z=(lookAt_z-camPos_z);

                double horiz_d = sqrt(x*x+z*z);

                pitch -= pitch_step*CV_PI/180.;
                if(pitch < -2*CV_PI)
                    pitch = 0;
                if(pitch > 2*CV_PI)
                    pitch = 0;

                x = sin(pitch)*horiz_d;
                z = sqrt(horiz_d*horiz_d - x*x);

                camPos_x = lookAt_x-x;
                camPos_z = lookAt_z-z;

                TransfCam2Orig = TransformationFromCamToOrigin(camPos_x, camPos_y, camPos_z, pitch, yaw);
                break;
            }
        }

        imshow("image", image);
    }
}

Mat PointCloudViewer::TransformationFromCamToOrigin(double camPos_x, double camPos_y, double camPos_z, double pitch, double yaw)
{
    Mat rot = Eular2Rot(yaw, pitch, 0).inv();

    Mat Tr = Mat::eye(4,4,CV_64F);
    Tr.at<double>(0,3) = -camPos_x; //x;
    Tr.at<double>(1,3) = -camPos_y; //y;
    Tr.at<double>(2,3) = -camPos_z; //z;
    for(int r=0; r<3; r++)
    {
        for(int c=0; c<3; c++)
        {
            Tr.at<double>(r,c) = rot.at<double>(r,c);
        }
    }

    return Tr;
}

Mat PointCloudViewer::Eular2Rot(double yaw,double pitch, double roll)
{
    Mat rot;

    double theta = yaw;
    double pusai = pitch;
    double phi = roll;

    double datax[3][3] = {{1.0,0.0,0.0},
    {0.0,cos(theta),-sin(theta)},
    {0.0,sin(theta),cos(theta)}};
    double datay[3][3] = {{cos(pusai),0.0,sin(pusai)},
    {0.0,1.0,0.0},
    {-sin(pusai),0.0,cos(pusai)}};
    double dataz[3][3] = {{cos(phi),-sin(phi),0.0},
    {sin(phi),cos(phi),0.0},
    {0.0,0.0,1.0}};
    Mat Rx(3,3,CV_64F,datax);
    Mat Ry(3,3,CV_64F,datay);
    Mat Rz(3,3,CV_64F,dataz);
    rot=Rz*Rx*Ry;

    return rot;
}

void PointCloudViewer::drawAxis(Mat &image, Mat K, Mat TransfCam2Orig, int lineThickness)
{
    Mat norm_axis_start, norm_axis_end, image_axis_start, image_axis_end;

    //draw x axis
    Mat axis_start = Mat::ones(4,1,CV_64F);
    axis_start.at<double>(0,0) = 0;
    axis_start.at<double>(1,0) = 0;
    axis_start.at<double>(2,0) = 0;
    Mat axis_end = Mat::ones(4,1,CV_64F);
    axis_end.at<double>(0,0) = 10;
    axis_end.at<double>(1,0) = 0;
    axis_end.at<double>(2,0) = 0;

    Mat projected_axis_start = TransfCam2Orig*axis_start;
    Mat projected_axis_end = TransfCam2Orig*axis_end;

    if(projected_axis_start.at<double>(2,0) > 0 && projected_axis_end.at<double>(2,0) > 0)
    {
        norm_axis_start = Mat::ones(3,1,CV_64F);
        norm_axis_end = Mat::ones(3,1,CV_64F);
        norm_axis_start.at<double>(0,0) = projected_axis_start.at<double>(0,0)/projected_axis_start.at<double>(2,0);
        norm_axis_start.at<double>(1,0) = projected_axis_start.at<double>(1,0)/projected_axis_start.at<double>(2,0);
        norm_axis_end.at<double>(0,0) = projected_axis_end.at<double>(0,0)/projected_axis_end.at<double>(2,0);
        norm_axis_end.at<double>(1,0) = projected_axis_end.at<double>(1,0)/projected_axis_end.at<double>(2,0);

        image_axis_start = K*norm_axis_start;
        image_axis_end = K*norm_axis_end;

        line(image,
        Point(round(image_axis_start.at<double>(0,0)),round(image_axis_start.at<double>(1,0))),
        Point(round(image_axis_end.at<double>(0,0)),round(image_axis_end.at<double>(1,0))),
        Scalar(0,255,0),
        lineThickness);
    }

    //draw x axis
    axis_start = Mat::ones(4,1,CV_64F);
    axis_start.at<double>(0,0) = 0;
    axis_start.at<double>(1,0) = 0;
    axis_start.at<double>(2,0) = 0;
    axis_end = Mat::ones(4,1,CV_64F);
    axis_end.at<double>(0,0) = 0;
    axis_end.at<double>(1,0) = 10;
    axis_end.at<double>(2,0) = 0;

    projected_axis_start = TransfCam2Orig*axis_start;
    projected_axis_end = TransfCam2Orig*axis_end;

    if(projected_axis_start.at<double>(2,0) > 0 && projected_axis_end.at<double>(2,0) > 0)
    {
        norm_axis_start = Mat::ones(3,1,CV_64F);
        norm_axis_end = Mat::ones(3,1,CV_64F);
        norm_axis_start.at<double>(0,0) = projected_axis_start.at<double>(0,0)/projected_axis_start.at<double>(2,0);
        norm_axis_start.at<double>(1,0) = projected_axis_start.at<double>(1,0)/projected_axis_start.at<double>(2,0);
        norm_axis_end.at<double>(0,0) = projected_axis_end.at<double>(0,0)/projected_axis_end.at<double>(2,0);
        norm_axis_end.at<double>(1,0) = projected_axis_end.at<double>(1,0)/projected_axis_end.at<double>(2,0);

        image_axis_start = K*norm_axis_start;
        image_axis_end = K*norm_axis_end;

        line(image,
        Point(round(image_axis_start.at<double>(0,0)),round(image_axis_start.at<double>(1,0))),
        Point(round(image_axis_end.at<double>(0,0)),round(image_axis_end.at<double>(1,0))),
        Scalar(255,0,0),
        lineThickness);
    }

    //draw x axis
    axis_start = Mat::ones(4,1,CV_64F);
    axis_start.at<double>(0,0) = 0;
    axis_start.at<double>(1,0) = 0;
    axis_start.at<double>(2,0) = 0;
    axis_end = Mat::ones(4,1,CV_64F);
    axis_end.at<double>(0,0) = 0;
    axis_end.at<double>(1,0) = 0;
    axis_end.at<double>(2,0) = 10;

    projected_axis_start = TransfCam2Orig*axis_start;
    projected_axis_end = TransfCam2Orig*axis_end;

    if(projected_axis_start.at<double>(2,0) > 0 && projected_axis_end.at<double>(2,0) > 0)
    {
        norm_axis_start = Mat::ones(3,1,CV_64F);
        norm_axis_end = Mat::ones(3,1,CV_64F);
        norm_axis_start.at<double>(0,0) = projected_axis_start.at<double>(0,0)/projected_axis_start.at<double>(2,0);
        norm_axis_start.at<double>(1,0) = projected_axis_start.at<double>(1,0)/projected_axis_start.at<double>(2,0);
        norm_axis_end.at<double>(0,0) = projected_axis_end.at<double>(0,0)/projected_axis_end.at<double>(2,0);
        norm_axis_end.at<double>(1,0) = projected_axis_end.at<double>(1,0)/projected_axis_end.at<double>(2,0);

        image_axis_start = K*norm_axis_start;
        image_axis_end = K*norm_axis_end;

        line(image,
        Point(round(image_axis_start.at<double>(0,0)),round(image_axis_start.at<double>(1,0))),
        Point(round(image_axis_end.at<double>(0,0)),round(image_axis_end.at<double>(1,0))),
        Scalar(0,0,255),
        lineThickness);
    }
}

void PointCloudViewer::drawText(Mat &image)
{
    stringstream ss_cam_pos_x;
    ss_cam_pos_x << camPos_x;
    string str_cam_pos_x = "camPosX: "+ss_cam_pos_x.str()+"mm";

    stringstream ss_cam_pos_y;
    ss_cam_pos_y << camPos_y;
    string str_cam_pos_y = "camPosY: "+ss_cam_pos_y.str()+"mm";

    stringstream ss_cam_pos_z;
    ss_cam_pos_z << camPos_z;
    string str_cam_pos_z = "camPosZ: "+ss_cam_pos_z.str()+"mm";

    stringstream ss_look_at_x;
    ss_look_at_x << lookAt_x;
    string str_look_at_x = "lookAtX: "+ss_look_at_x.str()+"mm";

    stringstream ss_look_at_y;
    ss_look_at_y << lookAt_y;
    string str_look_at_y = "lookAtY: "+ss_look_at_y.str()+"mm";

    stringstream ss_look_at_z;
    ss_look_at_z << lookAt_z;
    string str_look_at_z = "lookAtZ: "+ss_look_at_z.str()+"mm";

    stringstream ss_pitch;
    ss_pitch << pitch*180/CV_PI;
    string str_pitch = "pitch: "+ss_pitch.str()+"deg";

    stringstream ss_yaw;
    ss_yaw << yaw*180/CV_PI;
    string str_yaw = "yaw: "+ss_yaw.str()+"deg";

    int fontface = cv::FONT_HERSHEY_SIMPLEX;
    double scale = 0.4;
    int thickness = 1;
    int baseline = 0;

    double it=1;
    createTextLabel(image, str_cam_pos_x, Point(0.05*image_w, it*0.05*image_h), Scalar(255, 255, 255));

    it+=0.75;
    createTextLabel(image, str_cam_pos_y, Point(0.05*image_w, it*0.05*image_h), Scalar(255, 255, 255));

    it+=0.75;
    createTextLabel(image, str_cam_pos_z, Point(0.05*image_w, it*0.05*image_h), Scalar(255, 255, 255));

    it+=0.75;
    createTextLabel(image, str_look_at_x, Point(0.05*image_w, it*0.05*image_h), Scalar(255, 255, 255));

    it+=0.75;
    createTextLabel(image, str_look_at_y, Point(0.05*image_w, it*0.05*image_h), Scalar(255, 255, 255));

    it+=0.75;
    createTextLabel(image, str_look_at_z, Point(0.05*image_w, it*0.05*image_h), Scalar(255, 255, 255));

    it+=0.75;
    createTextLabel(image, str_pitch, Point(0.05*image_w, it*0.05*image_h), Scalar(255, 255, 255));

    it+=0.75;
    createTextLabel(image, str_yaw, Point(0.05*image_w, it*0.05*image_h), Scalar(255, 255, 255));

}

void PointCloudViewer::createTextLabel(Mat &image, string text, Point textPos, Scalar textColor)
{
    int fontface = cv::FONT_HERSHEY_SIMPLEX;
    double scale = 0.4;
    int thickness = 1;
    int baseline = 0;

    cv::Size textSize = cv::getTextSize(text, fontface, scale, thickness, &baseline);
    cv::rectangle(image, textPos + cv::Point(0, baseline), textPos + cv::Point(textSize.width, -textSize.height), background_color, CV_FILLED);
    cv::putText(image, text, textPos, fontface, scale, textColor, thickness, 8);
}