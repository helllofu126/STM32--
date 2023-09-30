using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace WpfApp1
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        private SerialPort port = null;
        private int Buf_MAX = 150;
        private byte[] buffer;
        private Thread thread;

        public MainWindow()
        {
            InitializeComponent();
            InitializeWindow();
        }


        /// <summary>
        /// 初始化窗口
        /// </summary>
        private void InitializeWindow()
        {
            //获取信息
            string[] portsName = SerialPort.GetPortNames();
            int[] baudRates = { 9600, 57600, 115200 };


            //令桌面控件显示信息
            combox_serialPorts.ItemsSource = portsName;
            combox_baudRate.ItemsSource = baudRates;
            combox_baudRate.SelectedIndex = 2;
            combox_serialPorts.SelectedIndex = 0;
        }

        /// <summary>
        /// 初始化串口
        /// </summary>
        /// <param name="portName">串口号</param>
        /// <param name="bautRate">波特率</param>
        private void InitPort(String portName, int bautRate)
        {
            if (!string.IsNullOrEmpty(portName))
            {
                buffer = new byte[Buf_MAX];
                port = new SerialPort(portName, bautRate);
                port.DataBits = 8;
                port.StopBits = StopBits.One;
                port.Parity = Parity.None;
                port.Open();
                //if (port.IsOpen) LogOut("串口打开成功！");
            }
            else
            {
                LogOut("请检查串口号!");
            }


        }



        /// <summary>
        /// 打印日志
        /// </summary>
        /// <param name="msg">日志内容</param>
        private void LogOut(object msg)
        {
            Dispatcher.Invoke(() =>
            {
                listbox_log.Items.Add(msg);
                if (listbox_log.Items.Count > 10)
                {
                    listbox_log.SelectedIndex = listbox_log.Items.Count - 1;
                    Decorator d = (Decorator)VisualTreeHelper.GetChild(listbox_log, 0);
                    ScrollViewer v = (ScrollViewer)d.Child;
                    v.ScrollToBottom();
                }
            });
        }


        private void Receve()
        {

            if (port != null && port.IsOpen)
            {
                btn_conn.Content = "关闭";

                LogOut("监控连接成功");
                LogOut("等待数据传输....");
                thread = new Thread(() =>
                {
                    while (true)
                    {
                        try
                        {
                            Thread.Sleep(300);
                            if (port.BytesToRead > 0)
                            {
                                string msg = port.ReadLine();
                                //port.Read(buffer, 0, Buf_MAX);
                                string[] data = msg.Split(',');
                                string[] data1 = data[0].Split(':');
                                if (data1[0].Equals("\"tem\""))
                                {
                                    LogOut(data[0]);
                                    LogOut(data[1]);
                                    LogOut(data[2]);
                                    LogOut(data[3]);
                                    LogOut(data[4]);
                                    LogOut("数据接收成功！！\n\n");

                                }
                                else
                                {
                                    LogOut(msg);

                                }
                            }
                        }
                        catch { }

                    }
                });
                thread.Start();


            }
            else
            {
                LogOut("监控连接失败！");
            }
        }

        private void btn_conn_Click(object sender, RoutedEventArgs e)
        {
            if (btn_conn.Content.ToString() == "连接")
            {

                InitPort(combox_serialPorts.Text, int.Parse(combox_baudRate.Text));
                Receve();

            }
            else
            {
                btn_conn.Content = "连接";
                port.Close();
                thread.Abort();
                port = null;
                LogOut("串口关闭成功！");
            }



        }

        private void Window_Closed(object sender, EventArgs e)
        {
            if (port!=null&&port.IsOpen)
            {
                port.Close();
                thread.Abort();
            }
        }
    }
}
