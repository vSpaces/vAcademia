using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;


namespace VisualWorldEditor
{
    public partial class MaterialForm : Form, IMessageFilter 
    {
        public class CustomComboBoxItem
        {
            private String m_value;
            private String m_text;


            public CustomComboBoxItem(String value, String text)
            {
                m_value = value;
                m_text = text;
            }

            public String Value
            {
                get
                {
                    return m_value;
                }
            }

            public String Text
            {
                get
                {
                    return m_text;
                }
            }

            public override string ToString()
            {
                return m_text;
            }
        };

        private Material m_material;
        private Object3D m_object;
        private MaterialFile m_materialFileDesc;

        private EditorTreeNode m_textureCheckComboValue;

        private System.Timers.Timer m_refreshTimer;

        private string m_lastObjectName;
        private string m_lastName;
        private int m_lastLOD;

        public MaterialForm()
        {
            InitializeComponent();

            m_material = new Material();
            m_object = Object3D.GetObject3D();

            m_object.NamesChanged += new ObjectEditedEventHandler(UpdateMaterial);

            texturesCheckCombo.TreeDropDown.AfterCheck += new TreeViewEventHandler(texturesCheckCombo_AfterCheck);            

            LoadTemplatesList();

            LoadTemplateItems();
            SetAddinsEnableState(false);

            m_textureCheckComboValue = null;
            m_materialFileDesc = null;

            m_refreshTimer = new System.Timers.Timer(10000);
            m_refreshTimer.Elapsed += new System.Timers.ElapsedEventHandler(TimedRefresh);

            m_lastName = "";
            m_lastLOD = 0;

            Application.AddMessageFilter(this);

            m_lastObjectName = "";
        }

        public bool PreFilterMessage(ref Message m) 
        {
            if (m.Msg == 0x20a) 
            {
                return true;
            }      

            return false;
        }

        public void UpdateConnection()
        {
            m_material.UpdateConnection();
        }
        
        private void LoadTemplatesList()
        {
            ArrayList templatesList = m_material.GetTemplatesList();

            for (int templateIndex = 0; templateIndex < templatesList.Count; templateIndex += 2)
            {
                templatesComboBox.Items.Add(templatesList[templateIndex]);
            }
        }

        private void LoadTemplateItems()
        {
            MaterialFile templateMaterial = m_material.GetTemplateItems();
            
            if (templateMaterial != null)
            {
                ArrayList templateItems = templateMaterial.GetParams();

                foreach (Hashtable item in templateItems)
                {
                    if (((String)item["paramName"] != "alphafunc") && ((String)item["paramName"] != "alphavalue") &&
                        ((String)item["paramName"] != "min_filter") && ((String)item["paramName"] != "mag_filter") &&
                        ((String)item["paramName"] != "anisotropy_power") && ((String)item["paramName"] != "mipmap_bias") &&
                        ((String)item["paramName"] != "mipmap"))
                    {
                        addinsComboBox.Items.Add(
                            new CustomComboBoxItem(
                                (String)item["paramName"],
                                m_material.GetParamTranslate((String)item["paramName"])));
                    }
                }
           

            }
        }

        private delegate void UpdateMaterialDelegate(Object sender, ObjectEditedEventArgs args);
        private void UpdateMaterial(Object sender, ObjectEditedEventArgs args)
        {
            if (this.InvokeRequired)
            {
                UpdateMaterialDelegate updateMaterial = new UpdateMaterialDelegate(UpdateMaterial);
                this.BeginInvoke(updateMaterial, new object[] { sender, args });
            }
            else
            {                
                if (m_lastObjectName == (String)args.EditParams[0])
                {
                    return;
                }
                m_lastObjectName = (String)args.EditParams[0];
                // сохраним материал при смене объекта
                SaveMaterial();

                texturesCheckCombo.TreeDropDown.Nodes.Clear();
                texturesCheckCombo.CheckedItems.Clear();
                texturesCheckCombo.Value = "";

                if ((bool)args.EditParams[1] || m_material.IsAvatar((String)args.EditParams[0]) || (Options.GetHost().IndexOf("virtuniver.loc") == -1
                    && Options.GetHost().IndexOf("192.168.0.2") == -1 && Options.GetHost().IndexOf("192.168.0.78") == -1))
                {
                    texturesCheckCombo.Enabled = false;
                    // куча объектов, очистим
                    m_material.SetNewObject("");
                    noMaterialInfo.Visible = false;
                }
                else
                {
                    m_material.SetNewObject((String)args.EditParams[0]);
                    ArrayList texturesList = m_material.GetTextures();

                    if (texturesList == null || texturesList.Count == 0)
                    {
                        texturesCheckCombo.Enabled = false;
                        btnTemplateCreate.Enabled = false;

                        noMaterialInfo.Visible = true;

                        return;
                    }

                    texturesCheckCombo.Enabled = true;

                    noMaterialInfo.Visible = false;

                    texturesCheckCombo.TreeDropDown.ShowNodeToolTips = true;

                    for (int lodIndex = 0; lodIndex < texturesList.Count; lodIndex++)
                    {
                        ArrayList lodTextures = (ArrayList)texturesList[lodIndex];

                        foreach (String textureName in lodTextures)
                        {
                            EditorTreeNode textureNode = new EditorTreeNode();
                            textureNode.Text = textureName;
                            textureNode.SetId(lodIndex);


                            textureNode.ToolTipText = "Для лода " + lodIndex.ToString();

                            if (m_material.IsUsingMaterial(textureName, lodIndex))
                            {
                                textureNode.Checked = true;
                                texturesCheckCombo.CheckedItems.Add(textureNode);
                            }
                            texturesCheckCombo.TreeDropDown.Nodes.Add(textureNode);
                        }
                    }

                    SelectLastTexture();
                }
            }
        }

        public void SaveMaterial()
        {
            if (texturesCheckCombo.Value != "" && m_materialFileDesc != null)
            {
                EditorTreeNode node = (EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode;

                m_material.SaveMaterial(node.Text, node.GetId(), m_materialFileDesc);
            }
        }

        private void MaterialForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            // сохраним материал при закрытии окна материалов
            SaveMaterial();

            // скрытие формы вместо закрытия
            e.Cancel = true;
            this.Hide();
        }


        private void FillForm(MaterialFile materialFile)
        {            
            if (materialFile != null)
            {
                EditorTreeNode node = (EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode;
                if (node != null)
                {
                    currentLod.Text = "Лод: " + node.GetId(); 
                }
                m_materialFileDesc = materialFile;
                
                ArrayList materialParams = materialFile.GetParams();
                // в соответсвии с файлом материала создаем контролы
                materialParamsPanel.SuspendLayout();
                materialParamsPanel.Controls.Clear();

                int height = 0;

                foreach (Hashtable param in materialParams)
                {
                    if ((String)param["paramType"] == "state")
                    {
                        String paramName = (String)param["paramName"];

                        if (paramName == "two_sided")
                        {
                            materialParamsPanel.Controls.AddRange(GetCheckBox(
                                paramName,
                                m_material.GetParamTranslate(paramName) + ":",
                                (String)param["value"],
                                new Point(0, height)
                                ));

                            height += 20;
                        }
                        else if (paramName == "billboard_orient")
                        {
                            materialParamsPanel.Controls.AddRange(GetCheckBox(
                                paramName,
                                m_material.GetParamTranslate(paramName) + ":",
                                (String)param["value"],
                                new Point(0, height)
                                ));

                            height += 20;
                        }
                        else if (paramName == "two_sided_transparency")
                        {
                            materialParamsPanel.Controls.AddRange(GetCheckBox(
                                paramName,
                                m_material.GetParamTranslate(paramName) + ":",
                                (String)param["value"],
                                new Point(0, height)
                                ));

                            height += 20;
                        }
                        else if (paramName == "blend")
                        {
                            materialParamsPanel.Controls.AddRange(GetCheckBox(
                                paramName,
                                m_material.GetParamTranslate(paramName) + ":",
                                (String)param["value"],
                                new Point(0, height)
                                ));

                            height += 20;
                        }
                        else if (paramName == "color")
                        {
                            materialParamsPanel.Controls.AddRange(GetColorDialog(
                                paramName,
                                m_material.GetParamTranslate(paramName) + ":",
                                (String)param["value"],
                                new Point(0, height)
                                ));

                            height += 20;
                        }
                        if (paramName == "min_filter")
                        {

                            ArrayList comboItems = new ArrayList();
                            comboItems.Add(new CustomComboBoxItem("nearest", "NEAREST"));
                            comboItems.Add(new CustomComboBoxItem("linear", "LINEAR"));

                            materialParamsPanel.Controls.AddRange(GetComboBox(
                               paramName,
                               m_material.GetParamTranslate(paramName) + ":",
                               comboItems,
                                (String)param["value"],
                               new Point(0, height)
                               ));

                            height += 40;
                        }
                        else if (paramName == "mipmap")
                        {

                            ArrayList comboItems = new ArrayList();
                            comboItems.Add(new CustomComboBoxItem("0", "нет"));
                            comboItems.Add(new CustomComboBoxItem("1", "ближайший"));
                            comboItems.Add(new CustomComboBoxItem("2", "усредненный"));

                            materialParamsPanel.Controls.AddRange(GetComboBox(
                               paramName,
                               m_material.GetParamTranslate(paramName) + ":",
                               comboItems,
                               (String)param["value"],
                               new Point(0, height)
                               ));

                            height += 40;
                        }

                        else if (paramName == "mipmap_bias")
                        {
                            materialParamsPanel.Controls.AddRange(GetTrackBar(
                                paramName,
                                m_material.GetParamTranslate(paramName) + ":",
                                "0.0",
                                "3.0",
                                 (String)param["value"],
                                new Point(0, height),
                                true
                                ));

                            height += 60;
                        }
                        else if (paramName == "mag_filter")
                        {
                            ArrayList comboItems = new ArrayList();
                            comboItems.Add(new CustomComboBoxItem("nearest", "NEAREST"));
                            comboItems.Add(new CustomComboBoxItem("linear", "LINEAR"));

                            materialParamsPanel.Controls.AddRange(GetComboBox(
                               paramName,
                               m_material.GetParamTranslate(paramName) + ":",
                               comboItems,
                               (String)param["value"],
                               new Point(0, height)
                               ));

                            height += 40;
                        }
                        else if (paramName == "anisotropy_power")
                        {
                            ArrayList comboItems = new ArrayList();
                            comboItems.Add(new CustomComboBoxItem("1", "1x"));
                            comboItems.Add(new CustomComboBoxItem("2", "2x"));
                            comboItems.Add(new CustomComboBoxItem("4", "4x"));
                            comboItems.Add(new CustomComboBoxItem("8", "8x"));
                            comboItems.Add(new CustomComboBoxItem("16", "16x"));

                            materialParamsPanel.Controls.AddRange(GetComboBox(
                               paramName,
                               m_material.GetParamTranslate(paramName) + ":",
                               comboItems,
                                (String)param["value"],
                               new Point(0, height)
                               ));

                            height += 40;
                        }
                        else if (paramName == "alphatest")
                        {
                            materialParamsPanel.Controls.AddRange(GetCheckBox(
                                paramName,
                                m_material.GetParamTranslate(paramName) + ":",
                                (String)param["value"],
                                new Point(0, height)
                                ));

                            height += 20;

                            for (int paramIndex = 0; paramIndex < materialParams.Count; paramIndex++)
                            {
                                if ((String)((Hashtable)materialParams[paramIndex])["paramType"] == "state")
                                {
                                    String subParamName = (String)((Hashtable)materialParams[paramIndex])["paramName"];

                                    if (subParamName == "alphavalue")
                                    {
                                        materialParamsPanel.Controls.AddRange(GetTrackBar(
                                            subParamName,
                                            m_material.GetParamTranslate(subParamName) + ":",
                                            "0",//(String)((Hashtable)materialParams[paramIndex])["valueMin"],
                                            "255",//(String)((Hashtable)materialParams[paramIndex])["valueMax"],
                                            (String)((Hashtable)materialParams[paramIndex])["value"],
                                            new Point(20, height),
                                            false
                                            ));

                                        height += 60;
                                    }
                                    else if (subParamName == "alphafunc")
                                    {
                                        ArrayList comboItems = new ArrayList();
                                        comboItems.Add(new CustomComboBoxItem("greater", "Больше"));
                                        comboItems.Add(new CustomComboBoxItem("equal", "Равенство"));
                                        comboItems.Add(new CustomComboBoxItem("less", "Меньше"));
                                        comboItems.Add(new CustomComboBoxItem("lessorequal", "Меньше или равно"));
                                        comboItems.Add(new CustomComboBoxItem("notequal", "Не равно"));
                                        comboItems.Add(new CustomComboBoxItem("greaterorequal", "Больше или равно"));
                                        comboItems.Add(new CustomComboBoxItem("always", "Всегда"));

                                        materialParamsPanel.Controls.AddRange(GetComboBox(
                                            subParamName,
                                            m_material.GetParamTranslate(subParamName) + ":",
                                            comboItems,
                                            (String)((Hashtable)materialParams[paramIndex])["value"],
                                            new Point(20, height)
                                            ));

                                        height += 40;
                                    }
                                }
                            }

                        }
                    }
                    else if ((String)param["paramType"] == "param")
                    {
                        String valueType = (String)param["valueType"];
                        String paramName = (String)param["paramName"];

                        if (valueType == "float")
                        {
                            materialParamsPanel.Controls.AddRange(GetTrackBar(
                                paramName,
                                m_material.GetParamTranslate(paramName) + ":",
                                (String)param["valueMin"],
                                (String)param["valueMax"],
                                (String)param["value"],
                                new Point(0, height),
                                true
                                ));

                            height += 60;
                        }
                        else if ((valueType == "texture") || (valueType == "cubemap"))
                        {
                            // слот текстурной карты
                            String level = (String)param["value"];

                            String textureSrc = GetTextureSrc(level);
                            if (textureSrc != "")
                            {
                                materialParamsPanel.Controls.AddRange(GetTextureSelector(
                                    paramName,
                                    m_material.GetParamTranslate(paramName) + ":",
                                    textureSrc,
                                    new Point(0, height)
                                    ));

                                height += 40;
                            }
                        }
                        else if (valueType == "vector")
                        {
                            // нету пока
                        }
                    }
                }

                materialParamsPanel.ResumeLayout();
            }
            else
            {
                materialParamsPanel.SuspendLayout();
                materialParamsPanel.Controls.Clear();
                materialParamsPanel.ResumeLayout();
            }
        }

        private Control[] GetTrackBar(String name, String text, String min, String max, String value, Point pos, bool floatValue)
        {
            int floatDot = 1000;

            Label trackBarLabel = new Label();
            trackBarLabel.Name = name + "_label";
            trackBarLabel.AutoSize = true;
            trackBarLabel.Text = text;
            trackBarLabel.Location = pos;

            TrackBar newTrackBar = new TrackBar();
            newTrackBar.Name = name;
            newTrackBar.Location = new Point(pos.X, pos.Y + trackBarLabel.PreferredHeight);
            newTrackBar.Size = new Size(184, 45);
            if (!floatValue)
            {
                newTrackBar.Minimum = Int32.Parse(min);
                newTrackBar.Maximum = Int32.Parse(max);
                newTrackBar.Value = Int32.Parse(value);
            }
            else
            {
                newTrackBar.Minimum = (Int32)(floatDot * float.Parse(min));
                newTrackBar.Maximum = (Int32)(floatDot * float.Parse(max));
                newTrackBar.Value = (Int32)(floatDot * float.Parse(value));
            }

            newTrackBar.TickFrequency = (newTrackBar.Maximum - newTrackBar.Minimum) / 20 + 1;

            Label minLabel = new Label();
            minLabel.AutoSize = true;
            minLabel.Name = name + "_min";
            minLabel.Text = min;
            minLabel.Location = new Point(pos.X + 12 - minLabel.PreferredWidth / 2, newTrackBar.Location.Y + 30);

            Label maxLabel = new Label();
            maxLabel.AutoSize = true;
            maxLabel.Name = name + "_max";
            maxLabel.Text = max;
            maxLabel.Location = new Point(pos.X + newTrackBar.Size.Width - 12 - maxLabel.PreferredWidth / 2, newTrackBar.Location.Y + 30);

            TextBox valueTextBox = new TextBox();
            valueTextBox.Name = name + "_value";
            valueTextBox.Text = value;
            valueTextBox.Location = new Point(pos.X + newTrackBar.Size.Width, newTrackBar.Location.Y + 3);

            if (!floatValue)
            {
                valueTextBox.MaxLength = max.Length + Convert.ToInt32(newTrackBar.Minimum < 0);
            }
            else
            {
                int maxLength = max.Length;
                int dotPosition = max.IndexOf(".");
                if (dotPosition != -1)
                {
                    maxLength = max.Substring(0, dotPosition).Length;
                }
                valueTextBox.MaxLength = maxLength + floatDot.ToString().Length + Convert.ToInt32(newTrackBar.Minimum < 0);
            }

            String lengthString = ((int)Math.Pow(10, valueTextBox.MaxLength - 1)).ToString();
            SizeF textBoxSize = this.CreateGraphics().MeasureString(lengthString, valueTextBox.Font);
            valueTextBox.Size = new Size((int)textBoxSize.Width, 20);

            newTrackBar.MouseCaptureChanged += delegate(Object sender, EventArgs args)
            {
                if (!((TrackBar)sender).Capture)
                {
                    // посылка данных при отпускании мышки
                    EditorTreeNode node = (EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode;
                    String materialName = m_material.GetMaterialNameFromTexture(node.Text, node.GetId());
                    m_materialFileDesc.ChangeParam(materialName, newTrackBar.Name, valueTextBox.Text);
                }
            };

            newTrackBar.ValueChanged += delegate(Object sender, EventArgs args)
            {
                if (!floatValue)
                {
                    valueTextBox.Text = ((TrackBar)sender).Value.ToString();
                }
                else
                {
                    valueTextBox.Text = (((float)((TrackBar)sender).Value) / floatDot).ToString();
                }

                if (((TrackBar)sender).Capture)
                {
                    // чтобы не отсылалось при каждом движении ползунка
                    return;
                }

                // посылает данные при изменение в текстовом поле
                EditorTreeNode node = (EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode;
                String materialName = m_material.GetMaterialNameFromTexture(node.Text, node.GetId());
                m_materialFileDesc.ChangeParam(materialName, newTrackBar.Name, valueTextBox.Text);
            };

            valueTextBox.KeyPress += delegate(Object sender, KeyPressEventArgs e)
            {
                if (!(char.IsDigit(e.KeyChar) || char.IsControl(e.KeyChar) || e.KeyChar == '-' || e.KeyChar == '.'))
                {
                    e.Handled = true;
                }

                if (e.KeyChar == (char)Keys.Return)
                {
                    e.Handled = true;
                    Int32 newValue;
                    float newValueF = float.NaN;
                    if ((Int32.TryParse(((TextBox)sender).Text, out newValue) && !floatValue) ||
                        (float.TryParse(((TextBox)sender).Text, out newValueF) && floatValue))
                    {
                        if (floatValue && !float.IsNaN(newValueF))
                        {
                            newValue = (int)(newValueF * floatDot);
                        }

                        if (newValue < newTrackBar.Minimum)
                        {
                            if (newTrackBar.Value == newTrackBar.Minimum)
                            {
                                //((TextBox)sender).Text = newTrackBar.Minimum.ToString();
                                ((TextBox)sender).Text = minLabel.Text;
                            }
                            else
                            {
                                newTrackBar.Value = newTrackBar.Minimum;
                            }
                        }
                        else if (newValue > newTrackBar.Maximum)
                        {
                            if (newTrackBar.Value == newTrackBar.Maximum)
                            {
                                //((TextBox)sender).Text = newTrackBar.Maximum.ToString();
                                ((TextBox)sender).Text = maxLabel.Text;
                            }
                            else
                            {
                                newTrackBar.Value = newTrackBar.Maximum;
                            }
                        }
                        else
                        {
                            newTrackBar.Value = newValue;
                        }
                    }
                    else
                    {
                        newTrackBar.Value = 0;
                    }
                }
            };

            return new Control[] { trackBarLabel, minLabel, maxLabel, newTrackBar, valueTextBox };
        }

        private Control[] GetComboBox(String name, String text, ArrayList items, String value, Point pos)
        {
            Label comboBoxLabel = new Label();
            comboBoxLabel.Name = name + "_label";
            comboBoxLabel.AutoSize = true;
            comboBoxLabel.Text = text;
            comboBoxLabel.Location = pos;

            ComboBox newComboBox = new ComboBox();
            newComboBox.Name = name;
            newComboBox.Size = new Size(184, 21);
            newComboBox.FlatStyle = FlatStyle.Flat;
            newComboBox.DropDownStyle = ComboBoxStyle.DropDownList;
            newComboBox.Location = new Point(pos.X + 3, pos.Y + comboBoxLabel.PreferredHeight);
            newComboBox.Items.AddRange(items.ToArray());
            for (int itemIndex = 0; itemIndex < items.Count; itemIndex++)
            {
                CustomComboBoxItem item = (CustomComboBoxItem)items[itemIndex];

                if (item.Value == value)
                {
                    newComboBox.SelectedIndex = itemIndex;
                    break;
                }
            }

            newComboBox.SelectedIndexChanged += delegate(Object sender, EventArgs e)
            {
                String selectedValue = ((CustomComboBoxItem)newComboBox.SelectedItem).Value;
                EditorTreeNode node = (EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode;
                String materialName = m_material.GetMaterialNameFromTexture(node.Text, node.GetId());
                m_materialFileDesc.ChangeParam(materialName, newComboBox.Name, selectedValue);
            };

            return new Control[] { comboBoxLabel, newComboBox };
        }

        private Control[] GetColorDialog(String name, String text, String value, Point pos)
        {
            Label colorTextLabel = new Label();
            colorTextLabel.AutoSize = true;
            colorTextLabel.Name = name;
            colorTextLabel.Text = text;
            colorTextLabel.Location = pos;

            ColorDialog colorDialog = new ColorDialog();

            colorDialog.FullOpen = true;
            colorDialog.SolidColorOnly = true;

            String[] colors = value.Split(new char[] { ';' }, 3);
            colorDialog.Color = Color.FromArgb(Int32.Parse(colors[0]),
                                                  Int32.Parse(colors[1]),
                                                  Int32.Parse(colors[2]));

            Label colorSelectLabel = new Label();
            colorSelectLabel.AutoSize = true;
            colorSelectLabel.BackColor = colorDialog.Color;
            colorSelectLabel.BorderStyle = BorderStyle.Fixed3D;
            colorSelectLabel.Location = new Point(pos.X + colorTextLabel.PreferredSize.Width + 5, pos.Y);
            colorSelectLabel.Name = name + "_select";
            colorSelectLabel.Text = "     ";
            colorSelectLabel.Cursor = Cursors.Hand;

            colorSelectLabel.Click += delegate(Object sender, EventArgs args)
            {
                DialogResult dlgResult = colorDialog.ShowDialog();
                if (dlgResult == DialogResult.OK)
                {
                    colorSelectLabel.BackColor = colorDialog.Color;
                    String colorValue = colorDialog.Color.R.ToString() + ";" +
                                        colorDialog.Color.G.ToString() + ";" +
                                        colorDialog.Color.B.ToString();
                    EditorTreeNode node = (EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode;
                    String materialName = m_material.GetMaterialNameFromTexture(node.Text, node.GetId());
                    m_materialFileDesc.ChangeParam(materialName, colorTextLabel.Name, colorValue);
                }
            };

            // отображаем установленный цвет

            return new Control[] { colorTextLabel, colorSelectLabel };
        }

        private Control[] GetCheckBox(String name, String text, String value, Point pos)
        {
            CheckBox newCheckBox = new CheckBox();

            newCheckBox.Name = name;
            newCheckBox.AutoSize = true;
            newCheckBox.CheckAlign = ContentAlignment.MiddleRight;
            newCheckBox.Location = pos;
            newCheckBox.Text = text;
            newCheckBox.UseVisualStyleBackColor = true;
            newCheckBox.Checked = bool.Parse(value);

            newCheckBox.CheckedChanged += delegate(Object sender, EventArgs e)
            {
                String checkValue = newCheckBox.Checked.ToString();
                EditorTreeNode node = (EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode;
                String materialName = m_material.GetMaterialNameFromTexture(node.Text, node.GetId());
                m_materialFileDesc.ChangeParam(materialName, newCheckBox.Name, checkValue);
            };

            return new Control[] { newCheckBox };
        }

        private Control[] GetTextureSelector(String name, String text, String value, Point pos)
        {
            Label textureLabel = new Label();
            textureLabel.AutoSize = true;
            textureLabel.Name = name + "_label";
            textureLabel.Text = text;
            textureLabel.Location = pos;

            TextBox textureTextBox = new TextBox();
            textureTextBox.Location = new Point(pos.X, pos.Y + 15);
            textureTextBox.Name = name;
            textureTextBox.Size = new Size(180, 20);
            textureTextBox.Text = value;

            Button textureButton = new Button();
            textureButton.Location = new Point(pos.X+185, pos.Y + 12);
            textureButton.Text = "Загрузить...";
            textureButton.Size = new Size(75,23);

            String textureSrc="";
            EditorTreeNode node = (EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode;

            textureTextBox.KeyPress += delegate(object sender, KeyPressEventArgs e)
            {
                if (e.KeyChar == (char)Keys.Return && textureTextBox.Text != "")
                {
                    textureSrc = textureTextBox.Text;
                    materialChangeParam(name, textureSrc,node);
                    e.Handled = true;
                }
            };

            textureButton.Click += delegate(object sender, EventArgs e)
            {
               OpenFileDialog openFileDialogTexture = new OpenFileDialog();
               openFileDialogTexture.Filter = "Файл текстуры|*.jpg; *.JPG;*.jpeg,*.JPEG;*.PNG;*.png";
               openFileDialogTexture.Title = "Выберите файл";

               if (openFileDialogTexture.ShowDialog() == DialogResult.OK)
               {
                   ArrayList resPath = m_material.GetResoursesPaths();
                   String texturesPath = (String)resPath[node.GetId()] + "/../textures/";
                   String textureFileName = openFileDialogTexture.FileName;

                   textureSrc = Path.GetFileName(textureFileName).ToUpper();
                   textureTextBox.Text = textureSrc;
                   File.Copy(textureFileName, texturesPath+textureSrc,true);

                   materialChangeParam(name,textureSrc, node);
               } 
            };


            return new Control[] { textureLabel, textureTextBox , textureButton};
        }

        private void materialChangeParam(String name, String src, EditorTreeNode node)
        {
            String materialName = m_material.GetMaterialNameFromTexture(node.Text, node.GetId());
            m_materialFileDesc.ChangeParam(materialName, name, src);
        }

        private void texturesCheckCombo_ValueChanged(object sender, EventArgs e)
        {
            if (m_materialFileDesc != null && m_textureCheckComboValue != null)            
            {
                EditorTreeNode node = (EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode;

                //m_material.SaveMaterial(node.Text, node.GetId(), m_materialFileDesc);
                if ((node == null) || (node.Text != m_textureCheckComboValue.Text) || (m_textureCheckComboValue.GetId() != node.GetId()))
                {
                m_material.SaveMaterial(m_textureCheckComboValue.Text, m_textureCheckComboValue.GetId(), m_materialFileDesc);
            }
            }

            if (texturesCheckCombo.Value != "")
            {
                templatesComboBox.Enabled = true;
                btnTemplateCreate.Enabled = true;
                // отобразить параметры материала текстуры
                // нет материала - нажать создать
                EditorTreeNode texture = (EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode;
                String textureName = texture.Text;

                Console.WriteLine(textureName);

                MaterialFile material = m_material.GetMaterial(textureName, texture.GetId());
                FillForm(material);
            }
            else
            {
                materialParamsPanel.Controls.Clear();
                templatesComboBox.SelectedIndex = -1;
                templatesComboBox.Enabled = false;
                btnTemplateCreate.Enabled = false;
            }

            m_textureCheckComboValue = (EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode;
        }

        private void btnTemplateCreate_Click(object sender, EventArgs e)
        {
            int index = templatesComboBox.SelectedIndex;
            if (index == -1)
            {
                return;
            }

            bool billboard = false;
            if (m_materialFileDesc != null)
            {
                billboard = m_materialFileDesc.isBillboard;
                m_materialFileDesc = m_material.GetTemplate(index);
                if (m_materialFileDesc != null)
                {
                    billboard = (billboard && !m_materialFileDesc.isBillboard) || (!billboard && m_materialFileDesc.isBillboard);
                }
                else
                {
                    MessageBox.Show("Шаблон не может быть загружен!");
                }
            }
            else
            {
                m_materialFileDesc = m_material.GetTemplate(index);
                if (m_materialFileDesc != null)
                {
                    billboard = m_materialFileDesc.isBillboard;
                }
                else
                {
                    MessageBox.Show("Шаблон не может быть загружен!");
                }
            }

            FillForm(m_materialFileDesc);

            EditorTreeNode node = (EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode;
            String materialName = m_material.GetMaterialNameFromTexture(node.Text, node.GetId());
            m_material.OnCreated(materialName, index);

            if (billboard)
            {
                // m_refreshTimer.Enabled = true;
                Commander.GetCommander().RefreshObjects();
                //btnTemplateCreate_Click(sender, e);
                //Commander.GetCommander().RefreshObjects();
            }

        }

        private delegate void TimeRefreshDelegate(object source, System.Timers.ElapsedEventArgs e);
        private void TimedRefresh(object source, System.Timers.ElapsedEventArgs e)
        {
            if (this.InvokeRequired)
            {
                TimeRefreshDelegate timedRefresh = new TimeRefreshDelegate(TimedRefresh);
                this.BeginInvoke(timedRefresh, new Object[] { source, e });
            }
            else
            {
                Commander.GetCommander().RefreshObjects();
                m_refreshTimer.Enabled = false;
            }
        }

        private void texturesCheckCombo_DropDownHided(object sender, EventArgs e)
        {
            // сохраним отмеченные текстуры, для которых используется материал
            ArrayList materials = new ArrayList();

            foreach (EditorTreeNode material in texturesCheckCombo.CheckedItems)
            {
                if (materials.Count <= material.GetId())
                {
                    for (int i = materials.Count; i <= material.GetId(); i++)
                    {
                        materials.Add(new ArrayList());
                    }
                }
                ((ArrayList)materials[material.GetId()]).Add(material.Text);
            }
            m_material.SaveUsingMaterials(materials);
        }

        private void texturesCheckCombo_AfterCheck(Object sender, TreeViewEventArgs e)
        {
            if (e.Action == TreeViewAction.ByMouse || e.Action == TreeViewAction.ByKeyboard)
            {
                EditorTreeNode selectedNode = (EditorTreeNode)e.Node;

                String checkState = "on";
                if (!selectedNode.Checked)
                {
                    checkState = "off";
                }
                else
                {
                    if (m_materialFileDesc != null && selectedNode != null)
                    {
                        m_material.SaveMaterial(selectedNode.Text, selectedNode.GetId(), m_materialFileDesc);
                    }
                }

                m_material.SwitchState(selectedNode.Text, selectedNode.GetId(), checkState);

                // сохраним отмеченную текстуру
                ArrayList materials = new ArrayList();

                if (materials.Count <= selectedNode.GetId())
                {
                    for (int i = materials.Count; i <= selectedNode.GetId(); i++)
                    {
                        materials.Add(new ArrayList());
                    }
                }
                ((ArrayList)materials[selectedNode.GetId()]).Add(selectedNode.Text);
                m_material.SaveUsingMaterials(materials);
            }

       


        }

        private void btnAddMaterialParam_Click(object sender, EventArgs e)
        {
            if (addinsComboBox.SelectedIndex == -1)
            {
                return;
            }

            Hashtable newParam = m_material.GetTemplateItems().GetParam(((CustomComboBoxItem)addinsComboBox.SelectedItem).Value);
            Hashtable newParam2 = null;
            Hashtable newParam3 = null;

             if (m_materialFileDesc.GetParam((String)newParam["paramName"]) == null) // такого параметра еще нет
            {
                if ((String)newParam["paramName"] == "anisotropy")
                {
                    if ( (m_materialFileDesc.GetParam("min_filter")== null)&&
                         (m_materialFileDesc.GetParam("mipmap")== null ) &&
                         (m_materialFileDesc.GetParam("mipmap_bias") == null) &&
                         (m_materialFileDesc.GetParam("mag_filter") == null) &&
                         (m_materialFileDesc.GetParam("anisotropy_power") == null))
                    {
                        m_materialFileDesc.AddParam("state",
                                      "min_filter",
                                      "linear", null, null, null);
                        m_materialFileDesc.AddParam("state",
                                          "mipmap",
                                           "2", null, null, null);
                        m_materialFileDesc.AddParam("state",
                                          "mipmap_bias",
                                          "0.0", "float", "0.0", "3.0");
                        m_materialFileDesc.AddParam("state",
                                          "mag_filter",
                                          "linear", null, null, null);
                        m_materialFileDesc.AddParam("state",
                                          "anisotropy_power",
                                          "1", null, null, null);
                    }
                    else // такие параметры уже есть
                    {
                        MessageBox.Show("Данные параметры уже присутствуют в материале", "Предупреждение", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                   
                }
                else 
                {
                    m_materialFileDesc.AddParam(newParam);
                }
                
                if ((String)newParam["paramName"] == "alphatest")
                {
                    newParam2 = m_material.GetTemplateItems().GetParam("alphafunc");
                    newParam3 = m_material.GetTemplateItems().GetParam("alphavalue");
                    m_materialFileDesc.AddParam(newParam2);
                    m_materialFileDesc.AddParam(newParam3);
                }
                FillForm(m_materialFileDesc);

                EditorTreeNode node = (EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode;
                String materialName = m_material.GetMaterialNameFromTexture(node.Text, node.GetId());

                if ((String)newParam["paramName"] != "anisotropy")
                {
                    m_materialFileDesc.ChangeParam(materialName, (String)newParam["paramName"], (String)newParam["value"]);
                }
                else
                {
                    m_materialFileDesc.ChangeParam(materialName, "min_filter", "linear");
                    m_materialFileDesc.ChangeParam(materialName, "mipmap", "2");
                    m_materialFileDesc.ChangeParam(materialName, "mipmap_bias", "0.0");
                    m_materialFileDesc.ChangeParam(materialName, "mag_filter", "linear");
                    m_materialFileDesc.ChangeParam(materialName, "anisotropy_power", "1");
                }
                if ((String)newParam["paramName"] == "alphatest")
                {
                    m_materialFileDesc.ChangeParam(materialName, (String)newParam2["paramName"], (String)newParam2["value"]);
                    m_materialFileDesc.ChangeParam(materialName, (String)newParam3["paramName"], (String)newParam3["value"]);
                }
            }
            else // такой параметр уже есть
            {
                MessageBox.Show("Данный параметр уже присутствует в материале", "Предупреждение", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void SetAddinsEnableState(bool enable)
        {
            if (enable && !addinsComboBox.Enabled)
            {
                addinsComboBox.Enabled = true;
                btnAddMaterialParam.Enabled = true;
            }
            else if (!enable && addinsComboBox.Enabled)
            {
                addinsComboBox.SelectedIndex = -1;
                addinsComboBox.Enabled = false;
                btnAddMaterialParam.Enabled = false;
            }
        }

        private void materialParamsPanel_ControlRemoved(object sender, ControlEventArgs e)
        {
            SetAddinsEnableState(false);
        }

        private void materialParamsPanel_ControlAdded(object sender, ControlEventArgs e)
        {
            SetAddinsEnableState(true);
        }

        private String GetTextureSrc(String level)
        {
            String src = "";

            ArrayList materialParams = m_materialFileDesc.GetParams();
            foreach (Hashtable param in materialParams)
            {
                if (((String)param["paramType"] == "texture" || (String)param["paramType"] == "cubemap") &&
                    (String)param["value"] == level)
                {
                    src = (String)param["paramName"];
                    break;
                }
            }

            return src;
        }

        private void texturesCheckCombo_Click(object sender, EventArgs e)
        {
            if (m_textureCheckComboValue != null)
            if (texturesCheckCombo.TreeDropDown.SelectedNode != null)
            if (m_textureCheckComboValue.GetId() != ((EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode).GetId())
            {
                texturesCheckCombo_ValueChanged(sender, e);
    }
        }

        private void texturesCheckCombo_CloseDropDown(object sender, UtilityLibrary.Combos.CustomCombo.EventArgsCloseDropDown e)
        {
            if (m_textureCheckComboValue != null)
            if (texturesCheckCombo.TreeDropDown.SelectedNode != null)
            if (m_textureCheckComboValue.GetId() != ((EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode).GetId())
            {
               texturesCheckCombo_ValueChanged(sender, e);
            }
        }
        
        public void SelectTexture(string name, int lod)
        {
            if (this.Visible == false)
            {
                return;
            }

            if (this.selectMaterialByClick.Checked == false)
            {
                return;
            }

            if ((name == null) || (name == ""))
            {
                return;
            }

            name = name.ToLower();

            if (texturesCheckCombo == null)
            {
                return;
            }

            if (texturesCheckCombo.TreeDropDown == null)
            {
                return;
            }

            if (texturesCheckCombo.TreeDropDown.Nodes == null)
            {
                return;
            }

            for (int i = 0; i < texturesCheckCombo.TreeDropDown.Nodes.Count; i++)
            {
                EditorTreeNode node = (EditorTreeNode)texturesCheckCombo.TreeDropDown.Nodes[i];
                if (node != null)
                if ((node.GetId() == lod) && (node.Text == name))
                {
                    texturesCheckCombo.TreeDropDown.SelectedNode = node;
                    texturesCheckCombo.TreeDropDown.TopNode = node;
                    texturesCheckCombo.TreeDropDown.Refresh();
                    texturesCheckCombo.TreeDropDown.Focus();                    
                    texturesCheckCombo_ValueChanged(null, null);
                    texturesCheckCombo.Value = node.Text;
                    break;
                }
            }

            m_lastName = name;
            m_lastLOD = lod;
        }

        private void SelectLastTexture()
        {
            SelectTexture(m_lastName, m_lastLOD);
        }

        private void incLodButton_Click(object sender, EventArgs e)
        {
            EditorTreeNode node = (EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode;
            if (node != null)
            {
                int id = node.GetId();
                string name = node.Text;

                for (int i = 0; i < texturesCheckCombo.TreeDropDown.Nodes.Count; i++)
                {
                    node = (EditorTreeNode)texturesCheckCombo.TreeDropDown.Nodes[i];
                    if ((node.GetId() == id + 1) && (node.Text == name))
                    {
                        texturesCheckCombo.TreeDropDown.SelectedNode = node;
                        texturesCheckCombo.Text = node.Text;
                        texturesCheckCombo_ValueChanged(sender, e);
                        texturesCheckCombo.Update();
                        break;
                    }
                }
            }
        }

        private void decLodButton_Click(object sender, EventArgs e)
        {
            EditorTreeNode node = (EditorTreeNode)texturesCheckCombo.TreeDropDown.SelectedNode;
            if (node != null)
            {
                int id = node.GetId();
                if (id == 0)
                {
                    return;
                }
                string name = node.Text;

                for (int i = 0; i < texturesCheckCombo.TreeDropDown.Nodes.Count; i++)
                {
                    node = (EditorTreeNode)texturesCheckCombo.TreeDropDown.Nodes[i];
                    if ((node.GetId() == id - 1) && (node.Text == name))
                    {
                        texturesCheckCombo.TreeDropDown.SelectedNode = node;
                        texturesCheckCombo.Text = node.Text;
                        texturesCheckCombo_ValueChanged(sender, e);
                        texturesCheckCombo.Update();
                        break;
                    }
                }
            }
        }


    }
}