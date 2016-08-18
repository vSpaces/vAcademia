using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using UtilityLibrary.Combos;

namespace VisualWorldEditor
{
    class EditorCheckCombo: UtilityLibrary.Combos.CheckCombo
    {
        public EditorCheckCombo()
        {
            
        }
        
       

        protected override string CalculateValue()
        {
            TreeNode selectedItem = this.TreeDropDown.SelectedNode;

            return selectedItem.Text;
        }
    }
}
