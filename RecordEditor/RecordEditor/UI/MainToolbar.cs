using System;
using RecordEditor;

namespace UI
{
    public partial class MainToolbar : FlashGenie.sePanel
    {
        public MainToolbar()
        {
            InitializeComponent();
        }

        private void btnCut_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.Assert(Program.context.recordController.CurrentRecord != null);
            Program.context.recordController.CurrentRecord.CutSelection();
        }

        private void btnCopy_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.Assert(Program.context.recordController.CurrentRecord != null);
            Program.context.recordController.CurrentRecord.CopySelection();
        }

        private void btnPaste_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.Assert(Program.context.recordController.CurrentRecord != null);
            Program.context.recordController.CurrentRecord.PasteSelection();
        }

        private void btnDelete_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Debug.Assert(Program.context.recordController.CurrentRecord != null);
            Program.context.recordController.CurrentRecord.DeleteSelection();
        }

        private void btnUndo_Click(object sender, EventArgs e)
        {
            
        }

        private void btnRedo_Click(object sender, EventArgs e)
        {

        }
    }
}
