using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace VisualWorldEditor
{
    public partial class LodInfo : Form
    {
        public const int LOD_INFO_ADD_SIMILAR_LOD = 1;
        public const int LOD_INFO_ADD_LIKE_OBJECT = 2;
        public const int LOD_INFO_EDIT = 3;

        public LodInfo(int mode)
        {
            InitializeComponent();
            distanceTBox.Text = "Введите дистанцию";
            if (mode != LOD_INFO_EDIT)
            {
                this.btnCreateLod.Text = "Создать";
            }
            else
            {
                this.btnCreateLod.Text = "Редактировать";
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        private void btnCreateLod_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        public int GetDistance()
        {
            int distance;
            if(!int.TryParse(this.distanceTBox.Text, out distance))
            {
                return -1;
            }

            return distance;
        }

        public bool GetShadow()
        {
            return shadowCheckBox.Checked;
        }

        public void SetShadow(bool state)
        {
            shadowCheckBox.Checked = state;
        }

        public void SetDistance(int distance)
        {
            distanceTBox.Text = distance.ToString();
        }
    }
}