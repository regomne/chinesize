namespace PackGsPak
{
    partial class MainWnd
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.buttonBrowsePack = new System.Windows.Forms.Button();
            this.textBoxPacket = new System.Windows.Forms.TextBox();
            this.radioButtonExp = new System.Windows.Forms.RadioButton();
            this.radioButtonPack = new System.Windows.Forms.RadioButton();
            this.buttonBrowseDir = new System.Windows.Forms.Button();
            this.textBoxDir = new System.Windows.Forms.TextBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.buttonStart = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.buttonBrowsePack);
            this.groupBox1.Controls.Add(this.textBoxPacket);
            this.groupBox1.Location = new System.Drawing.Point(12, 33);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(437, 53);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "原始封包";
            // 
            // buttonBrowsePack
            // 
            this.buttonBrowsePack.Location = new System.Drawing.Point(398, 20);
            this.buttonBrowsePack.Name = "buttonBrowsePack";
            this.buttonBrowsePack.Size = new System.Drawing.Size(33, 21);
            this.buttonBrowsePack.TabIndex = 1;
            this.buttonBrowsePack.Text = "...";
            this.buttonBrowsePack.UseVisualStyleBackColor = true;
            this.buttonBrowsePack.Click += new System.EventHandler(this.buttonBrowsePack_Click);
            // 
            // textBoxPacket
            // 
            this.textBoxPacket.Location = new System.Drawing.Point(13, 21);
            this.textBoxPacket.Name = "textBoxPacket";
            this.textBoxPacket.Size = new System.Drawing.Size(379, 21);
            this.textBoxPacket.TabIndex = 0;
            // 
            // radioButtonExp
            // 
            this.radioButtonExp.AutoSize = true;
            this.radioButtonExp.Location = new System.Drawing.Point(13, 11);
            this.radioButtonExp.Name = "radioButtonExp";
            this.radioButtonExp.Size = new System.Drawing.Size(47, 16);
            this.radioButtonExp.TabIndex = 1;
            this.radioButtonExp.TabStop = true;
            this.radioButtonExp.Text = "解包";
            this.radioButtonExp.UseVisualStyleBackColor = true;
            // 
            // radioButtonPack
            // 
            this.radioButtonPack.AutoSize = true;
            this.radioButtonPack.Location = new System.Drawing.Point(85, 12);
            this.radioButtonPack.Name = "radioButtonPack";
            this.radioButtonPack.Size = new System.Drawing.Size(47, 16);
            this.radioButtonPack.TabIndex = 2;
            this.radioButtonPack.TabStop = true;
            this.radioButtonPack.Text = "封包";
            this.radioButtonPack.UseVisualStyleBackColor = true;
            // 
            // buttonBrowseDir
            // 
            this.buttonBrowseDir.Location = new System.Drawing.Point(398, 20);
            this.buttonBrowseDir.Name = "buttonBrowseDir";
            this.buttonBrowseDir.Size = new System.Drawing.Size(33, 21);
            this.buttonBrowseDir.TabIndex = 1;
            this.buttonBrowseDir.Text = "...";
            this.buttonBrowseDir.UseVisualStyleBackColor = true;
            this.buttonBrowseDir.Click += new System.EventHandler(this.buttonBrowseDir_Click);
            // 
            // textBoxDir
            // 
            this.textBoxDir.Location = new System.Drawing.Point(13, 21);
            this.textBoxDir.Name = "textBoxDir";
            this.textBoxDir.Size = new System.Drawing.Size(379, 21);
            this.textBoxDir.TabIndex = 0;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.buttonBrowseDir);
            this.groupBox2.Controls.Add(this.textBoxDir);
            this.groupBox2.Location = new System.Drawing.Point(13, 92);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(437, 53);
            this.groupBox2.TabIndex = 2;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "目录";
            // 
            // buttonStart
            // 
            this.buttonStart.Location = new System.Drawing.Point(336, 6);
            this.buttonStart.Name = "buttonStart";
            this.buttonStart.Size = new System.Drawing.Size(112, 27);
            this.buttonStart.TabIndex = 3;
            this.buttonStart.Text = "干活";
            this.buttonStart.UseVisualStyleBackColor = true;
            this.buttonStart.Click += new System.EventHandler(this.buttonStart_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.Filter = "Pak文件(*.pak)|*.pak";
            this.openFileDialog1.Title = "打开原始脚本";
            // 
            // folderBrowserDialog1
            // 
            this.folderBrowserDialog1.Description = "指定解包/封包目录";
            // 
            // MainWnd
            // 
            this.AcceptButton = this.buttonStart;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(461, 155);
            this.Controls.Add(this.buttonStart);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.radioButtonPack);
            this.Controls.Add(this.radioButtonExp);
            this.Controls.Add(this.groupBox1);
            this.Name = "MainWnd";
            this.Text = "GsWin Packer";
            this.Load += new System.EventHandler(this.MainWnd_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button buttonBrowsePack;
        private System.Windows.Forms.TextBox textBoxPacket;
        private System.Windows.Forms.RadioButton radioButtonExp;
        private System.Windows.Forms.RadioButton radioButtonPack;
        private System.Windows.Forms.Button buttonBrowseDir;
        private System.Windows.Forms.TextBox textBoxDir;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button buttonStart;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog1;
    }
}

