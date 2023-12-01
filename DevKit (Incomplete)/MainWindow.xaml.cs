using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Windows;

namespace flowerbankdebugger
{
    public partial class MainWindow : Window
    {
        private TcpListener tcpListener;
        private Thread listenerThread;

        public MainWindow()
        {
            InitializeComponent();
            StartTcpListener();
        }

        private void StartTcpListener()
        {
            tcpListener = new TcpListener(IPAddress.Any, 5000); // Listen on port 5000
            listenerThread = new Thread(ListenForClients);
            listenerThread.Start();
        }

        private void ListenForClients()
        {
            tcpListener.Start();

            while (true)
            {
                TcpClient client = tcpListener.AcceptTcpClient();
                Thread clientThread = new Thread(HandleClientComm);
                clientThread.Start(client);
            }
        }

        private void HandleClientComm(object client)
        {
            TcpClient tcpClient = (TcpClient)client;
            NetworkStream clientStream = tcpClient.GetStream();
            byte[] message = new byte[4096];
            int bytesRead;

            while (true)
            {
                bytesRead = 0;
                try
                {
                    bytesRead = clientStream.Read(message, 0, 4096);
                }
                catch
                {
                    break;
                }
                if (bytesRead == 0)
                {
                    break;
                }

                string receivedMessage = Encoding.ASCII.GetString(message, 0, bytesRead);
                Dispatcher.Invoke(() => {
                    DebugTextBox.AppendText(receivedMessage + Environment.NewLine);
                });
            }

            tcpClient.Close();
        }

        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);
            if (tcpListener != null)
            {
                tcpListener.Stop();
            }
            if (listenerThread != null)
            {
                listenerThread.Abort();
            }
        }
    }
}
