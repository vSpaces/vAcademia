using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace VisualWorldEditor
{
    public partial class LibraryAddingForm : Form
    {
        private ArrayList m_libStruct;

        public LibraryAddingForm()
        {
            InitializeComponent();
        }

        public void FormOff()
        {
            this.DialogResult = DialogResult.Cancel;
            this.Hide();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            FormOff();
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            if(categoryCombo.SelectedIndex == -1 && categoryTBox.Text == "")
            {
                MessageBox.Show("Выберите категорию или введите название новой", "Категория", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            if (subcategoryCombo.SelectedIndex == -1 && subcategoryTBox.Text == "")
            {
                MessageBox.Show("Выберите подкатегорию или введите название новой", "Подкатегория", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            if(objectNameTBox.Text == "")
            {
                MessageBox.Show("Введите название для данного объекта", "Имя объекта", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            this.DialogResult = DialogResult.OK;            
            this.Close();
        }

        private void FillCategoryCombo(ArrayList libStruct)
        {
            m_libStruct = libStruct;
            categoryCombo.Items.Clear();
            for(int categoryIndex = 0; categoryIndex < libStruct.Count; categoryIndex++)
            {
                categoryCombo.Items.Add(((ArrayList)libStruct[categoryIndex])[0]);
            }
        }

        public DialogResult ShowAddingDialog(ArrayList libStruct, ref String category, ref String subcategory, ref String name)
        {
            FillCategoryCombo(libStruct);
            this.ShowDialog();

            if (this.categoryTBox.Text != "")
            {
                category = this.categoryTBox.Text;
                this.categoryTBox.Text = "";
            }
            else
            {
                category = this.categoryCombo.Text;
            }
            categoryCombo.SelectedIndex = -1;

            if (this.subcategoryTBox.Text != "")
            {
                subcategory = this.subcategoryTBox.Text;
                this.subcategoryTBox.Text = "";
            }
            else
            {
                subcategory = this.subcategoryCombo.Text;
            }
            subcategoryCombo.SelectedIndex = -1;

            name = this.objectNameTBox.Text;
            this.objectNameTBox.Text = "";

            return this.DialogResult;
        }

        private void categoryCombo_SelectedIndexChanged(object sender, EventArgs e)
        {
            subcategoryCombo.Items.Clear();

            if(categoryCombo.SelectedIndex != -1)
            {
                ArrayList subcategoryList = (ArrayList)m_libStruct[categoryCombo.SelectedIndex];

                for(int subcategoryIndex = 1; subcategoryIndex < subcategoryList.Count; subcategoryIndex++)
                {
                    subcategoryCombo.Items.Add(subcategoryList[subcategoryIndex]);
                }
            }
        }
    }
}