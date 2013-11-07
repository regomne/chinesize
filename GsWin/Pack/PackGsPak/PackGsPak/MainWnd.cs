using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace PackGsPak
{
    public partial class MainWnd : Form
    {
        public MainWnd()
        {
            InitializeComponent();
        }

        private void buttonBrowsePack_Click(object sender, EventArgs e)
        {
            openFileDialog1.ShowDialog();
            textBoxPacket.Text = openFileDialog1.FileName;
        }

        private void buttonBrowseDir_Click(object sender, EventArgs e)
        {
            folderBrowserDialog1.ShowDialog();
            textBoxDir.Text = folderBrowserDialog1.SelectedPath;
        }

        private void MainWnd_Load(object sender, EventArgs e)
        {
            radioButtonExp.Checked = true;
            //radioButtonPack.Checked = true;
        }

        private void buttonStart_Click(object sender, EventArgs e)
        {
            if (!File.Exists(textBoxPacket.Text))
            {
                MessageBox.Show("指定的封包不存在！", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            GsWinPakMgr pakMgr = new GsWinPakMgr();
            if (radioButtonPack.Checked)
            {
                if (!Directory.Exists(textBoxDir.Text))
                {
                    MessageBox.Show("指定的目录不存在！", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                try
                {
                    bool ret = pakMgr.Pack(textBoxDir.Text, textBoxPacket.Text);
                    MessageBox.Show("封包完成！");
                }
                catch (FormatException)
                {
                    MessageBox.Show("封包格式错误！");
                }
                catch (Exception ex)
                {
                    MessageBox.Show("封包失败！错误消息：\n" + ex.Message);
                }
            }
            else if (radioButtonExp.Checked)
            {
                if (!Directory.Exists(textBoxDir.Text))
                    Directory.CreateDirectory(textBoxDir.Text);
                try
                {
                    bool ret = pakMgr.Unpack(textBoxDir.Text, textBoxPacket.Text);
                    MessageBox.Show("解包完成！");
                }
                catch (FormatException)
                {
                    MessageBox.Show("封包格式错误！");
                }
                catch (Exception ex)
                {
                    MessageBox.Show("封包失败！错误消息：\n" + ex.Message);
                }
            }
        }

    }
}
