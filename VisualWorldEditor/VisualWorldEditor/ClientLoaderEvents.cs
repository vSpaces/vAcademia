using System;
using System.Collections.Generic;
using System.Text;

namespace VisualWorldEditor
{
    public delegate void ClientStartedEventHandler(Object sender, ClientEventArgs args);
    public delegate void ClientStoppedEventHandler(Object sender, ClientEventArgs args);

    public class ClientEventArgs
    {
        private Object args;

        public ClientEventArgs(Object _args)
        {
            args = _args;
        }

        public Object Args
        {
            get { return (args); }
        }
    };

    partial class ClientLoader
    {
        public event ClientStartedEventHandler ClientStarted;
        public event ClientStoppedEventHandler ClientStopped;

        protected virtual void OnClientStarted(ClientEventArgs args)
        {
            if (ClientStarted != null)
            {
                ClientStarted(this, args);
            }
        }

        protected virtual void OnClientStopped(ClientEventArgs args)
        {
            if (ClientStopped != null)
            {
                ClientStopped(this, args);
            }
        }


    }
}
