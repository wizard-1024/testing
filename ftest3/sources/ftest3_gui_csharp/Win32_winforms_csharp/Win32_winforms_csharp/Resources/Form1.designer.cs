namespace Win32_winforms_csharp
{
    partial class Form1
    {
        /// <summary>
        /// Требуется переменная конструктора.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Освободить все используемые ресурсы.
        /// </summary>
        /// <param name="disposing">истинно, если управляемый ресурс должен быть удален; иначе ложно.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Код, автоматически созданный конструктором форм Windows

        /// <summary>
        /// Обязательный метод для поддержки конструктора - не изменяйте
        /// содержимое данного метода при помощи редактора кода.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.radioButtonFileIoTesting = new System.Windows.Forms.RadioButton();
            this.radioButtonMemoryOnlyTesting = new System.Windows.Forms.RadioButton();
            this.buttonRun = new System.Windows.Forms.Button();
            this.buttonExit = new System.Windows.Forms.Button();
            this.textBoxCommandLine = new System.Windows.Forms.TextBox();
            this.labelCmdLine = new System.Windows.Forms.Label();
            this.groupBoxBuffers = new System.Windows.Forms.GroupBox();
            this.labelBufSize = new System.Windows.Forms.Label();
            this.textBoxBufSize = new System.Windows.Forms.TextBox();
            this.checkBoxFillRandom = new System.Windows.Forms.CheckBox();
            this.groupBoxGeneralOptions = new System.Windows.Forms.GroupBox();
            this.label2 = new System.Windows.Forms.Label();
            this.labelPassesNum = new System.Windows.Forms.Label();
            this.textBoxThreadsNum = new System.Windows.Forms.TextBox();
            this.textBoxPassesNum = new System.Windows.Forms.TextBox();
            this.checkBoxVerbose = new System.Windows.Forms.CheckBox();
            this.checkBoxTimeProfiling = new System.Windows.Forms.CheckBox();
            this.checkBoxLoopForever = new System.Windows.Forms.CheckBox();
            this.groupBoxFiles = new System.Windows.Forms.GroupBox();
            this.labelFilesNum = new System.Windows.Forms.Label();
            this.labelWorkDir = new System.Windows.Forms.Label();
            this.labelFileSize = new System.Windows.Forms.Label();
            this.textBoxFilesNum = new System.Windows.Forms.TextBox();
            this.textBoxFileSize = new System.Windows.Forms.TextBox();
            this.textBoxWorkDir = new System.Windows.Forms.TextBox();
            this.groupBoxMemoryTestOptions = new System.Windows.Forms.GroupBox();
            this.labelAutoBufSize = new System.Windows.Forms.Label();
            this.textBoxAutoBufSize = new System.Windows.Forms.TextBox();
            this.checkBoxAutoAllMemoryTesting = new System.Windows.Forms.CheckBox();
            this.checkBoxMemoryOnlyTest = new System.Windows.Forms.CheckBox();
            this.buttonAbout = new System.Windows.Forms.Button();
            this.buttonHelp = new System.Windows.Forms.Button();
            this.groupBoxBuffers.SuspendLayout();
            this.groupBoxGeneralOptions.SuspendLayout();
            this.groupBoxFiles.SuspendLayout();
            this.groupBoxMemoryTestOptions.SuspendLayout();
            this.SuspendLayout();
            // 
            // radioButtonFileIoTesting
            // 
            this.radioButtonFileIoTesting.AutoSize = true;
            this.radioButtonFileIoTesting.Location = new System.Drawing.Point(23, 24);
            this.radioButtonFileIoTesting.Name = "radioButtonFileIoTesting";
            this.radioButtonFileIoTesting.Size = new System.Drawing.Size(119, 21);
            this.radioButtonFileIoTesting.TabIndex = 0;
            this.radioButtonFileIoTesting.TabStop = true;
            this.radioButtonFileIoTesting.Text = "File I/O testing";
            this.radioButtonFileIoTesting.UseVisualStyleBackColor = true;
            this.radioButtonFileIoTesting.CheckedChanged += new System.EventHandler(this.radioButtonFileIoTesting_CheckedChanged);
            // 
            // radioButtonMemoryOnlyTesting
            // 
            this.radioButtonMemoryOnlyTesting.AutoSize = true;
            this.radioButtonMemoryOnlyTesting.Location = new System.Drawing.Point(23, 67);
            this.radioButtonMemoryOnlyTesting.Name = "radioButtonMemoryOnlyTesting";
            this.radioButtonMemoryOnlyTesting.Size = new System.Drawing.Size(155, 21);
            this.radioButtonMemoryOnlyTesting.TabIndex = 2;
            this.radioButtonMemoryOnlyTesting.TabStop = true;
            this.radioButtonMemoryOnlyTesting.Text = "Memory testing only";
            this.radioButtonMemoryOnlyTesting.UseVisualStyleBackColor = true;
            this.radioButtonMemoryOnlyTesting.CheckedChanged += new System.EventHandler(this.radioButtonMemoryOnlyTesting_CheckedChanged);
            // 
            // buttonRun
            // 
            this.buttonRun.Location = new System.Drawing.Point(775, 64);
            this.buttonRun.Name = "buttonRun";
            this.buttonRun.Size = new System.Drawing.Size(75, 23);
            this.buttonRun.TabIndex = 3;
            this.buttonRun.Text = "Run";
            this.buttonRun.UseVisualStyleBackColor = true;
            this.buttonRun.Click += new System.EventHandler(this.buttonRun_Click);
            // 
            // buttonExit
            // 
            this.buttonExit.Location = new System.Drawing.Point(775, 465);
            this.buttonExit.Name = "buttonExit";
            this.buttonExit.Size = new System.Drawing.Size(75, 28);
            this.buttonExit.TabIndex = 5;
            this.buttonExit.Text = "Exit";
            this.buttonExit.UseVisualStyleBackColor = true;
            this.buttonExit.Click += new System.EventHandler(this.buttonExit_click);
            // 
            // textBoxCommandLine
            // 
            this.textBoxCommandLine.Location = new System.Drawing.Point(305, 40);
            this.textBoxCommandLine.Multiline = true;
            this.textBoxCommandLine.Name = "textBoxCommandLine";
            this.textBoxCommandLine.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBoxCommandLine.Size = new System.Drawing.Size(434, 47);
            this.textBoxCommandLine.TabIndex = 6;
            this.textBoxCommandLine.TextChanged += new System.EventHandler(this.textBoxCommandLine_TextChanged);
            // 
            // labelCmdLine
            // 
            this.labelCmdLine.AutoSize = true;
            this.labelCmdLine.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.labelCmdLine.Location = new System.Drawing.Point(302, 20);
            this.labelCmdLine.Name = "labelCmdLine";
            this.labelCmdLine.Size = new System.Drawing.Size(114, 17);
            this.labelCmdLine.TabIndex = 7;
            this.labelCmdLine.Text = "Command Line";
            // 
            // groupBoxBuffers
            // 
            this.groupBoxBuffers.Controls.Add(this.labelBufSize);
            this.groupBoxBuffers.Controls.Add(this.textBoxBufSize);
            this.groupBoxBuffers.Controls.Add(this.checkBoxFillRandom);
            this.groupBoxBuffers.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.groupBoxBuffers.Location = new System.Drawing.Point(31, 126);
            this.groupBoxBuffers.Name = "groupBoxBuffers";
            this.groupBoxBuffers.Size = new System.Drawing.Size(402, 102);
            this.groupBoxBuffers.TabIndex = 8;
            this.groupBoxBuffers.TabStop = false;
            this.groupBoxBuffers.Text = "Buffer(s)";
            // 
            // labelBufSize
            // 
            this.labelBufSize.AutoSize = true;
            this.labelBufSize.Location = new System.Drawing.Point(76, 70);
            this.labelBufSize.Name = "labelBufSize";
            this.labelBufSize.Size = new System.Drawing.Size(155, 17);
            this.labelBufSize.TabIndex = 2;
            this.labelBufSize.Text = "(-s) buffer size (MB)";
            // 
            // textBoxBufSize
            // 
            this.textBoxBufSize.Location = new System.Drawing.Point(6, 65);
            this.textBoxBufSize.Name = "textBoxBufSize";
            this.textBoxBufSize.Size = new System.Drawing.Size(64, 22);
            this.textBoxBufSize.TabIndex = 1;
            this.textBoxBufSize.TextChanged += new System.EventHandler(this.textBoxBufSize_TextChanged);
            // 
            // checkBoxFillRandom
            // 
            this.checkBoxFillRandom.AutoSize = true;
            this.checkBoxFillRandom.Location = new System.Drawing.Point(6, 21);
            this.checkBoxFillRandom.Name = "checkBoxFillRandom";
            this.checkBoxFillRandom.Size = new System.Drawing.Size(195, 21);
            this.checkBoxFillRandom.TabIndex = 0;
            this.checkBoxFillRandom.Text = "(-r) fill buffer randomly";
            this.checkBoxFillRandom.UseVisualStyleBackColor = true;
            this.checkBoxFillRandom.CheckStateChanged += new System.EventHandler(this.checkBoxFillRandom_CheckStateChanged);
            // 
            // groupBoxGeneralOptions
            // 
            this.groupBoxGeneralOptions.Controls.Add(this.label2);
            this.groupBoxGeneralOptions.Controls.Add(this.labelPassesNum);
            this.groupBoxGeneralOptions.Controls.Add(this.textBoxThreadsNum);
            this.groupBoxGeneralOptions.Controls.Add(this.textBoxPassesNum);
            this.groupBoxGeneralOptions.Controls.Add(this.checkBoxVerbose);
            this.groupBoxGeneralOptions.Controls.Add(this.checkBoxTimeProfiling);
            this.groupBoxGeneralOptions.Controls.Add(this.checkBoxLoopForever);
            this.groupBoxGeneralOptions.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.groupBoxGeneralOptions.Location = new System.Drawing.Point(31, 257);
            this.groupBoxGeneralOptions.Name = "groupBoxGeneralOptions";
            this.groupBoxGeneralOptions.Size = new System.Drawing.Size(402, 188);
            this.groupBoxGeneralOptions.TabIndex = 9;
            this.groupBoxGeneralOptions.TabStop = false;
            this.groupBoxGeneralOptions.Text = "General Options";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(140, 121);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(149, 17);
            this.label2.TabIndex = 10;
            this.label2.Text = "(-j) threads number";
            // 
            // labelPassesNum
            // 
            this.labelPassesNum.AutoSize = true;
            this.labelPassesNum.Location = new System.Drawing.Point(140, 91);
            this.labelPassesNum.Name = "labelPassesNum";
            this.labelPassesNum.Size = new System.Drawing.Size(150, 17);
            this.labelPassesNum.TabIndex = 8;
            this.labelPassesNum.Text = "(-p) passes number";
            // 
            // textBoxThreadsNum
            // 
            this.textBoxThreadsNum.Location = new System.Drawing.Point(22, 116);
            this.textBoxThreadsNum.Name = "textBoxThreadsNum";
            this.textBoxThreadsNum.Size = new System.Drawing.Size(100, 22);
            this.textBoxThreadsNum.TabIndex = 7;
            this.textBoxThreadsNum.TextChanged += new System.EventHandler(this.textBoxThreadsNum_TextChanged);
            // 
            // textBoxPassesNum
            // 
            this.textBoxPassesNum.Location = new System.Drawing.Point(22, 88);
            this.textBoxPassesNum.Name = "textBoxPassesNum";
            this.textBoxPassesNum.Size = new System.Drawing.Size(104, 22);
            this.textBoxPassesNum.TabIndex = 5;
            this.textBoxPassesNum.TextChanged += new System.EventHandler(this.textBoxPassesNum_TextChanged);
            // 
            // checkBoxVerbose
            // 
            this.checkBoxVerbose.AutoSize = true;
            this.checkBoxVerbose.Location = new System.Drawing.Point(22, 161);
            this.checkBoxVerbose.Name = "checkBoxVerbose";
            this.checkBoxVerbose.Size = new System.Drawing.Size(170, 21);
            this.checkBoxVerbose.TabIndex = 4;
            this.checkBoxVerbose.Text = "(-v) verbose output";
            this.checkBoxVerbose.UseVisualStyleBackColor = true;
            this.checkBoxVerbose.CheckedChanged += new System.EventHandler(this.checkBoxVerbose_CheckedChanged);
            // 
            // checkBoxTimeProfiling
            // 
            this.checkBoxTimeProfiling.AutoSize = true;
            this.checkBoxTimeProfiling.Location = new System.Drawing.Point(22, 51);
            this.checkBoxTimeProfiling.Name = "checkBoxTimeProfiling";
            this.checkBoxTimeProfiling.Size = new System.Drawing.Size(157, 21);
            this.checkBoxTimeProfiling.TabIndex = 2;
            this.checkBoxTimeProfiling.Text = "(-Z) time profiling";
            this.checkBoxTimeProfiling.UseVisualStyleBackColor = true;
            this.checkBoxTimeProfiling.CheckStateChanged += new System.EventHandler(this.checkBoxTimeProfiling_CheckStateChanged);
            // 
            // checkBoxLoopForever
            // 
            this.checkBoxLoopForever.AutoSize = true;
            this.checkBoxLoopForever.Location = new System.Drawing.Point(22, 24);
            this.checkBoxLoopForever.Name = "checkBoxLoopForever";
            this.checkBoxLoopForever.Size = new System.Drawing.Size(145, 21);
            this.checkBoxLoopForever.TabIndex = 0;
            this.checkBoxLoopForever.Text = "(-l) loop forever";
            this.checkBoxLoopForever.UseVisualStyleBackColor = true;
            this.checkBoxLoopForever.CheckStateChanged += new System.EventHandler(this.checkBoxLoopForever_CheckStateChanged);
            // 
            // groupBoxFiles
            // 
            this.groupBoxFiles.Controls.Add(this.labelFilesNum);
            this.groupBoxFiles.Controls.Add(this.labelWorkDir);
            this.groupBoxFiles.Controls.Add(this.labelFileSize);
            this.groupBoxFiles.Controls.Add(this.textBoxFilesNum);
            this.groupBoxFiles.Controls.Add(this.textBoxFileSize);
            this.groupBoxFiles.Controls.Add(this.textBoxWorkDir);
            this.groupBoxFiles.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.groupBoxFiles.Location = new System.Drawing.Point(483, 136);
            this.groupBoxFiles.Name = "groupBoxFiles";
            this.groupBoxFiles.Size = new System.Drawing.Size(394, 150);
            this.groupBoxFiles.TabIndex = 10;
            this.groupBoxFiles.TabStop = false;
            this.groupBoxFiles.Text = "File(s)";
            // 
            // labelFilesNum
            // 
            this.labelFilesNum.AutoSize = true;
            this.labelFilesNum.Location = new System.Drawing.Point(141, 121);
            this.labelFilesNum.Name = "labelFilesNum";
            this.labelFilesNum.Size = new System.Drawing.Size(185, 17);
            this.labelFilesNum.TabIndex = 9;
            this.labelFilesNum.Text = "(-n) files/buffers number";
            // 
            // labelWorkDir
            // 
            this.labelWorkDir.AutoSize = true;
            this.labelWorkDir.Location = new System.Drawing.Point(178, 36);
            this.labelWorkDir.Name = "labelWorkDir";
            this.labelWorkDir.Size = new System.Drawing.Size(149, 17);
            this.labelWorkDir.TabIndex = 7;
            this.labelWorkDir.Text = "(-t) working catalog";
            // 
            // labelFileSize
            // 
            this.labelFileSize.AutoSize = true;
            this.labelFileSize.Location = new System.Drawing.Point(141, 86);
            this.labelFileSize.Name = "labelFileSize";
            this.labelFileSize.Size = new System.Drawing.Size(131, 17);
            this.labelFileSize.TabIndex = 5;
            this.labelFileSize.Text = "(-f) file size (MB)";
            // 
            // textBoxFilesNum
            // 
            this.textBoxFilesNum.Location = new System.Drawing.Point(23, 121);
            this.textBoxFilesNum.Name = "textBoxFilesNum";
            this.textBoxFilesNum.Size = new System.Drawing.Size(100, 22);
            this.textBoxFilesNum.TabIndex = 4;
            this.textBoxFilesNum.TextChanged += new System.EventHandler(this.textBoxFilesNum_TextChanged);
            // 
            // textBoxFileSize
            // 
            this.textBoxFileSize.Location = new System.Drawing.Point(23, 83);
            this.textBoxFileSize.Name = "textBoxFileSize";
            this.textBoxFileSize.Size = new System.Drawing.Size(100, 22);
            this.textBoxFileSize.TabIndex = 2;
            this.textBoxFileSize.TextChanged += new System.EventHandler(this.textBoxFileSize_TextChanged);
            // 
            // textBoxWorkDir
            // 
            this.textBoxWorkDir.Location = new System.Drawing.Point(23, 33);
            this.textBoxWorkDir.Multiline = true;
            this.textBoxWorkDir.Name = "textBoxWorkDir";
            this.textBoxWorkDir.Size = new System.Drawing.Size(138, 33);
            this.textBoxWorkDir.TabIndex = 0;
            this.textBoxWorkDir.TextChanged += new System.EventHandler(this.textBoxWorkDir_TextChanged);
            // 
            // groupBoxMemoryTestOptions
            // 
            this.groupBoxMemoryTestOptions.Controls.Add(this.labelAutoBufSize);
            this.groupBoxMemoryTestOptions.Controls.Add(this.textBoxAutoBufSize);
            this.groupBoxMemoryTestOptions.Controls.Add(this.checkBoxAutoAllMemoryTesting);
            this.groupBoxMemoryTestOptions.Controls.Add(this.checkBoxMemoryOnlyTest);
            this.groupBoxMemoryTestOptions.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.groupBoxMemoryTestOptions.Location = new System.Drawing.Point(483, 308);
            this.groupBoxMemoryTestOptions.Name = "groupBoxMemoryTestOptions";
            this.groupBoxMemoryTestOptions.Size = new System.Drawing.Size(394, 137);
            this.groupBoxMemoryTestOptions.TabIndex = 12;
            this.groupBoxMemoryTestOptions.TabStop = false;
            this.groupBoxMemoryTestOptions.Text = "Memory test options";
            // 
            // labelAutoBufSize
            // 
            this.labelAutoBufSize.AutoSize = true;
            this.labelAutoBufSize.Location = new System.Drawing.Point(141, 108);
            this.labelAutoBufSize.Name = "labelAutoBufSize";
            this.labelAutoBufSize.Size = new System.Drawing.Size(193, 17);
            this.labelAutoBufSize.TabIndex = 4;
            this.labelAutoBufSize.Text = "(-b) auto buffer size (MB)";
            // 
            // textBoxAutoBufSize
            // 
            this.textBoxAutoBufSize.Location = new System.Drawing.Point(23, 108);
            this.textBoxAutoBufSize.Name = "textBoxAutoBufSize";
            this.textBoxAutoBufSize.Size = new System.Drawing.Size(100, 22);
            this.textBoxAutoBufSize.TabIndex = 3;
            this.textBoxAutoBufSize.TextChanged += new System.EventHandler(this.textBoxAutoBufSize_TextChanged);
            // 
            // checkBoxAutoAllMemoryTesting
            // 
            this.checkBoxAutoAllMemoryTesting.AutoSize = true;
            this.checkBoxAutoAllMemoryTesting.Location = new System.Drawing.Point(23, 70);
            this.checkBoxAutoAllMemoryTesting.Name = "checkBoxAutoAllMemoryTesting";
            this.checkBoxAutoAllMemoryTesting.Size = new System.Drawing.Size(232, 21);
            this.checkBoxAutoAllMemoryTesting.TabIndex = 2;
            this.checkBoxAutoAllMemoryTesting.Text = "(-B) auto all memory testing";
            this.checkBoxAutoAllMemoryTesting.UseVisualStyleBackColor = true;
            this.checkBoxAutoAllMemoryTesting.CheckedChanged += new System.EventHandler(this.checkBoxAutoAllMemoryTesting_CheckedChanged);
            // 
            // checkBoxMemoryOnlyTest
            // 
            this.checkBoxMemoryOnlyTest.AutoSize = true;
            this.checkBoxMemoryOnlyTest.Location = new System.Drawing.Point(26, 35);
            this.checkBoxMemoryOnlyTest.Name = "checkBoxMemoryOnlyTest";
            this.checkBoxMemoryOnlyTest.Size = new System.Drawing.Size(185, 21);
            this.checkBoxMemoryOnlyTest.TabIndex = 0;
            this.checkBoxMemoryOnlyTest.Text = "(-e) memory only test";
            this.checkBoxMemoryOnlyTest.UseVisualStyleBackColor = true;
            this.checkBoxMemoryOnlyTest.CheckedChanged += new System.EventHandler(this.checkBoxMemoryOnlyTest_CheckedChanged);
            // 
            // buttonAbout
            // 
            this.buttonAbout.Location = new System.Drawing.Point(23, 465);
            this.buttonAbout.Name = "buttonAbout";
            this.buttonAbout.Size = new System.Drawing.Size(75, 28);
            this.buttonAbout.TabIndex = 13;
            this.buttonAbout.Text = "About";
            this.buttonAbout.UseVisualStyleBackColor = true;
            this.buttonAbout.Click += new System.EventHandler(this.buttonAbout_Click);
            // 
            // buttonHelp
            // 
            this.buttonHelp.Location = new System.Drawing.Point(135, 465);
            this.buttonHelp.Name = "buttonHelp";
            this.buttonHelp.Size = new System.Drawing.Size(75, 28);
            this.buttonHelp.TabIndex = 15;
            this.buttonHelp.Text = "Help";
            this.buttonHelp.UseVisualStyleBackColor = true;
            this.buttonHelp.Click += new System.EventHandler(this.buttonHelp_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(889, 505);
            this.Controls.Add(this.buttonHelp);
            this.Controls.Add(this.buttonAbout);
            this.Controls.Add(this.groupBoxMemoryTestOptions);
            this.Controls.Add(this.groupBoxFiles);
            this.Controls.Add(this.groupBoxGeneralOptions);
            this.Controls.Add(this.groupBoxBuffers);
            this.Controls.Add(this.labelCmdLine);
            this.Controls.Add(this.textBoxCommandLine);
            this.Controls.Add(this.buttonExit);
            this.Controls.Add(this.buttonRun);
            this.Controls.Add(this.radioButtonMemoryOnlyTesting);
            this.Controls.Add(this.radioButtonFileIoTesting);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Form1";
            this.Text = "Form1";
            this.groupBoxBuffers.ResumeLayout(false);
            this.groupBoxBuffers.PerformLayout();
            this.groupBoxGeneralOptions.ResumeLayout(false);
            this.groupBoxGeneralOptions.PerformLayout();
            this.groupBoxFiles.ResumeLayout(false);
            this.groupBoxFiles.PerformLayout();
            this.groupBoxMemoryTestOptions.ResumeLayout(false);
            this.groupBoxMemoryTestOptions.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.RadioButton radioButtonFileIoTesting;
        private System.Windows.Forms.RadioButton radioButtonMemoryOnlyTesting;
        private System.Windows.Forms.Button buttonRun;
        private System.Windows.Forms.Button buttonExit;
        private System.Windows.Forms.TextBox textBoxCommandLine;
        private System.Windows.Forms.Label labelCmdLine;
        private System.Windows.Forms.GroupBox groupBoxBuffers;
        private System.Windows.Forms.Label labelBufSize;
        private System.Windows.Forms.TextBox textBoxBufSize;
        private System.Windows.Forms.CheckBox checkBoxFillRandom;
        private System.Windows.Forms.GroupBox groupBoxGeneralOptions;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label labelPassesNum;
        private System.Windows.Forms.TextBox textBoxThreadsNum;
        private System.Windows.Forms.TextBox textBoxPassesNum;
        private System.Windows.Forms.CheckBox checkBoxVerbose;
        private System.Windows.Forms.CheckBox checkBoxTimeProfiling;
        private System.Windows.Forms.CheckBox checkBoxLoopForever;
        private System.Windows.Forms.GroupBox groupBoxFiles;
        private System.Windows.Forms.GroupBox groupBoxMemoryTestOptions;
        private System.Windows.Forms.TextBox textBoxFilesNum;
        private System.Windows.Forms.TextBox textBoxFileSize;
        private System.Windows.Forms.TextBox textBoxWorkDir;
        private System.Windows.Forms.Label labelFilesNum;
        private System.Windows.Forms.Label labelWorkDir;
        private System.Windows.Forms.Label labelFileSize;
        private System.Windows.Forms.Label labelAutoBufSize;
        private System.Windows.Forms.TextBox textBoxAutoBufSize;
        private System.Windows.Forms.CheckBox checkBoxAutoAllMemoryTesting;
        private System.Windows.Forms.CheckBox checkBoxMemoryOnlyTest;
        private System.Windows.Forms.Button buttonAbout;
        private System.Windows.Forms.Button buttonHelp;
    }
}

