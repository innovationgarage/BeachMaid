// Simple Player sample application
// AForge.NET framework
// http://www.aforgenet.com/framework/
//
// Copyright © AForge.NET, 2006-2011
// contacts@aforgenet.com
//

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

using AForge.Video;
using AForge.Video.DirectShow;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;

namespace Player
{
    public partial class MainForm : Form
    {
        byte[,] previous = null;
        private int lastx;
        private int lasty;

        // Class constructor
        public MainForm( )
        {
            InitializeComponent( );
        }

        private void MainForm_FormClosing( object sender, FormClosingEventArgs e )
        {
            CloseCurrentVideoSource( );
        }

        // "Exit" menu item clicked
        private void exitToolStripMenuItem_Click( object sender, EventArgs e )
        {
            this.Close( );
        }

        // Open local video capture device
        private void localVideoCaptureDeviceToolStripMenuItem_Click( object sender, EventArgs e )
        {
            VideoCaptureDeviceForm form = new VideoCaptureDeviceForm( );

            if ( form.ShowDialog( this ) == DialogResult.OK )
            {
                // create video source
                VideoCaptureDevice videoSource = form.VideoDevice;

                // open it
                OpenVideoSource( videoSource );
            }
        }

        // Open video file using DirectShow
        private void openVideofileusingDirectShowToolStripMenuItem_Click( object sender, EventArgs e )
        {
            if ( openFileDialog.ShowDialog( ) == DialogResult.OK )
            {
                // create video source
                FileVideoSource fileSource = new FileVideoSource( openFileDialog.FileName );

                // open it
                OpenVideoSource( fileSource );
            }
        }

        // Open JPEG URL
        private void openJPEGURLToolStripMenuItem_Click( object sender, EventArgs e )
        {
            URLForm form = new URLForm( );

            form.Description = "Enter URL of an updating JPEG from a web camera:";
            form.URLs = new string[]
				{
					"http://195.243.185.195/axis-cgi/jpg/image.cgi?camera=1",
				};

            if ( form.ShowDialog( this ) == DialogResult.OK )
            {
                // create video source
                JPEGStream jpegSource = new JPEGStream( form.URL );

                // open it
                OpenVideoSource( jpegSource );
            }
        }

        // Open MJPEG URL
        private void openMJPEGURLToolStripMenuItem_Click( object sender, EventArgs e )
        {
            URLForm form = new URLForm( );

            form.Description = "Enter URL of an MJPEG video stream:";
            form.URLs = new string[]
				{
					"http://195.243.185.195/axis-cgi/mjpg/video.cgi?camera=4",
					"http://195.243.185.195/axis-cgi/mjpg/video.cgi?camera=3",
				};

            if ( form.ShowDialog( this ) == DialogResult.OK )
            {
                // create video source
                MJPEGStream mjpegSource = new MJPEGStream( form.URL );

                // open it
                OpenVideoSource( mjpegSource );
            }
        }

        // Capture 1st display in the system
        private void capture1stDisplayToolStripMenuItem_Click( object sender, EventArgs e )
        {
            OpenVideoSource( new ScreenCaptureStream( Screen.AllScreens[0].Bounds, 100 ) );
        }

        // Open video source
        private void OpenVideoSource( IVideoSource source )
        {
            // set busy cursor
            this.Cursor = Cursors.WaitCursor;

            // stop current video source
            CloseCurrentVideoSource( );

            // start new video source
            videoSourcePlayer.VideoSource = source;
            videoSourcePlayer.Start( );


            this.Cursor = Cursors.Default;
        }

        // Close video source if it is running
        private void CloseCurrentVideoSource( )
        {
            if ( videoSourcePlayer.VideoSource != null )
            {
                videoSourcePlayer.SignalToStop( );

                // wait ~ 3 seconds
                for ( int i = 0; i < 30; i++ )
                {
                    if ( !videoSourcePlayer.IsRunning )
                        break;
                    System.Threading.Thread.Sleep( 100 );
                }

                if ( videoSourcePlayer.IsRunning )
                {
                    videoSourcePlayer.Stop( );
                }

                videoSourcePlayer.VideoSource = null;
            }
        }
        public static Bitmap ResizeImage(Image image, int width, int height)
        {
            var destRect = new Rectangle(0, 0, width, height);
            var destImage = new Bitmap(width, height);

            destImage.SetResolution(image.HorizontalResolution, image.VerticalResolution);

            using (var graphics = Graphics.FromImage(destImage))
            {
                graphics.CompositingMode = CompositingMode.SourceCopy;
                graphics.CompositingQuality = CompositingQuality.HighSpeed;
                graphics.InterpolationMode = InterpolationMode.NearestNeighbor;
                graphics.SmoothingMode = SmoothingMode.None;
                graphics.PixelOffsetMode = PixelOffsetMode.None;

                using (var wrapMode = new ImageAttributes())
                {
                    wrapMode.SetWrapMode(WrapMode.TileFlipXY);
                    graphics.DrawImage(image, destRect, 0, 0, image.Width, image.Height, GraphicsUnit.Pixel, wrapMode);
                }
            }

            return destImage;
        }
        // New frame received by the player
        private void videoSourcePlayer_NewFrame(object sender, ref Bitmap image)
        {
            /*DateTime now = DateTime.Now;
            Graphics g = Graphics.FromImage( image );

            // paint current time
            SolidBrush brush = new SolidBrush( Color.Red );
            g.DrawString( now.ToString( ), this.Font, brush, new PointF( 5, 5 ) );
            brush.Dispose( );

            g.Dispose( );*/

            const int size = 15;
            const int sign_size = 2;
            Bitmap temp = ResizeImage(new Bitmap((Image)image.Clone()), size, size);

 
            // Create matrix
            var current = new byte[size, size];

            for (int x = 0; x < size; x++)
                for (int y = 0; y < size; y++)
                    current[x, y] = ConvertToByte(temp.GetPixel(x, y));
            //previous = new int[size, size];

            var g = Graphics.FromImage(temp);

            const int rectangleSize = 2;
            var rx = (size + rectangleSize) / 2;
            var ry = rx;

            if (previous == null)
            {



            }
            else
            {
                // Find previous in current

                switch (GetDirection(current, previous))
                {
                    case ContentAlignment.TopLeft: rx = 0; ry = 0; break;
                    case ContentAlignment.TopCenter: ry = 0; break;
                    case ContentAlignment.TopRight: rx = size- rectangleSize; ry = 0; break;
                    case ContentAlignment.MiddleLeft: rx = 0; break;
                    case ContentAlignment.MiddleRight: rx = size- rectangleSize; break;
                    case ContentAlignment.BottomLeft: rx = 0; ry = size - rectangleSize; break;
                    case ContentAlignment.BottomCenter: ry = size - rectangleSize; break;
                    case ContentAlignment.BottomRight: rx = size - rectangleSize; ry = size - rectangleSize; break;
                }
            }

            lasty= rx- (size + rectangleSize) / 2;
            lastx = ry- (size + rectangleSize) / 2;

            g.FillRectangle(Brushes.Red, rx, ry, rectangleSize, rectangleSize);
            previous = current;
            g.Dispose();

            pictureBoxTemp.Image = temp;

        }

        private void MoveCursor(int x, int y)
        {
            // Set the Current cursor, move the cursor's Position,
            // and set its clipping rectangle to the form. 

            this.Cursor = new Cursor(Cursor.Current.Handle);
            Cursor.Position = new Point(Cursor.Position.X - x, Cursor.Position.Y - y);
            Cursor.Clip = new Rectangle(this.Location, this.Size);
        }

        private ContentAlignment GetDirection(byte[,] current, byte[,] previous)
        {
            int bestx = 0, besty = 0, bestscore = int.MaxValue, size = current.GetLength(0);
            const int maxoffset = 1;
            for (int x = -maxoffset; x <= maxoffset; x++)
                for (int y = -maxoffset; y <= maxoffset; y++)
                {
                    int score = 0;

                    for (int i = maxoffset; i < size - maxoffset; i++)
                        for (int j = maxoffset; j < size - maxoffset; j++)
                        {
                            score += Math.Abs(previous[i, j] - current[i+x, j+y]);
                            Debug.WriteLine("Comparing " + i + "," + j + " to " + (i + x) + " " + (j + y));
                        }

                    if (score < bestscore || (x==0 && y==0 && score <= bestscore))
                    {
                        bestx = x;
                        besty = y;
                        bestscore = score;
                    }

                    /*if (score == 0)
                        break; // Perfect match*/
                }

            switch (bestx)
            {
                case -1:
                    switch (besty)
                    {
                        case -1: return ContentAlignment.TopLeft;
                        case 0: return ContentAlignment.TopCenter;
                        case 1: return ContentAlignment.TopRight;

                    }
                    break;

                case 0:
                    switch (besty)
                    {
                        case -1: return ContentAlignment.MiddleLeft;
                        case 0: return ContentAlignment.MiddleCenter;
                        case 1: return ContentAlignment.MiddleRight;

                    }
                    break;

                case 1:
                    switch (besty)
                    {
                        case -1: return ContentAlignment.BottomLeft;
                        case 0: return ContentAlignment.BottomCenter;
                        case 1: return ContentAlignment.BottomRight;

                    }
                    break;
            }
            return ContentAlignment.MiddleCenter;
        }

        private byte ConvertToByte(Color color)
        {
            return (byte)((color.R + color.B + color.G) / 3);
        }

        private void timerMouse_Tick(object sender, EventArgs e)
        {
            if(videoSourcePlayer.VideoSource !=null && videoSourcePlayer.VideoSource.IsRunning)
                MoveCursor(lastx, lasty);
        }
    }
}
