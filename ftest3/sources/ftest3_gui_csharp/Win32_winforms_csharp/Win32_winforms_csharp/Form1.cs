using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Reflection;

namespace Win32_winforms_csharp
{

    public partial class Form1 : Form
    {
        int verbose = 1;
        int random_fill = 1;
        int loop_forever = 0;
        int time_profiling = 1;
        int memory_test_only = 0;
        int autodetect_memory_test = 0;

        ulong pass_num = 1;
        int threads_num = 1;
        ulong file_size = 16;
        int buffers_num = 1;
        long buf_size = 4;
        long auto_buf_size = 1000;

        string temp_dir;
        string program_path;
        string command_line;
        string user_command_line;

        //public Button newPanelButton = new Button();

        private void EnableMemoryTestItems()
        {
            checkBoxMemoryOnlyTest.Enabled = true;
            checkBoxAutoAllMemoryTesting.Enabled = true;
            textBoxAutoBufSize.Enabled = true;
            textBoxFileSize.Enabled = false;
            textBoxWorkDir.Enabled = false;
        }


        private void DisableMemoryTestItems()
        {
            checkBoxMemoryOnlyTest.Enabled = false;
            checkBoxAutoAllMemoryTesting.Enabled = false;
            textBoxAutoBufSize.Enabled = false;
            textBoxFileSize.Enabled = true;
            textBoxWorkDir.Enabled = true;
        }


        private void BuildCommandLine()
        {
            program_path = "ftest3.exe";
            string text;

            text = program_path;
            text += "  ";
            if (verbose == 1) text += " -v";
            if (random_fill == 1) text += " -r";
            if (loop_forever == 1) text += " -l";
            if (time_profiling == 1) text += " -Z";

             if (pass_num > 0) {
               if (loop_forever == 0) {
                 text += " -p ";
                 text += Convert.ToString(pass_num);
               }
            }

            if (threads_num > 0)
            {
                text += " -j ";
                text += Convert.ToString(threads_num);
            }

            if (buffers_num > 0)
            {
                text += " -n ";
                text += Convert.ToString(buffers_num);
            }

            if (buf_size > 0)
            {
                text += " -s ";
                text += Convert.ToString(buf_size);
            }

            if (memory_test_only == 1) {
                 text += " -e ";
                 if (autodetect_memory_test == 1) {
                    text += " -B ";
                    if (auto_buf_size > 0) {
                       text += " -b ";
                       text += Convert.ToString(auto_buf_size);
                    }
                 }
            }
            else {
                if (file_size > 0) {
                   text += " -f ";
                   text += Convert.ToString(file_size);
                }
                text += " -t ";
                text += temp_dir;
            }

            command_line = text;
        }


        private void DisplayCommandLine()
        {
            textBoxCommandLine.Text = command_line;
        }


        public Form1()
        {
            InitializeComponent();

            this.Text = "Ftest3 GUI Interface C#";
            this.StartPosition = FormStartPosition.CenterScreen;

            // init all elements to defaults
            if (verbose == 0) checkBoxVerbose.Checked = false;
            else checkBoxVerbose.Checked = true;

            if (random_fill == 0) checkBoxFillRandom.Checked = false;
            else checkBoxFillRandom.Checked = true;

            if (loop_forever == 0) checkBoxLoopForever.Checked = false;
            else checkBoxLoopForever.Checked = true;

            if (time_profiling == 0) checkBoxTimeProfiling.Checked = false;
            else checkBoxTimeProfiling.Checked = true;

            if (memory_test_only == 0) checkBoxMemoryOnlyTest.Checked = false;
            else checkBoxMemoryOnlyTest.Checked = true;

            if (autodetect_memory_test == 0) checkBoxAutoAllMemoryTesting.Checked = false;
            else checkBoxAutoAllMemoryTesting.Checked = true;

            textBoxBufSize.Text = Convert.ToString(buf_size);
            textBoxAutoBufSize.Text = Convert.ToString(auto_buf_size);
            textBoxThreadsNum.Text = Convert.ToString(threads_num);
            textBoxFileSize.Text = Convert.ToString(file_size);
            textBoxFilesNum.Text = Convert.ToString(buffers_num);
            textBoxPassesNum.Text = Convert.ToString(pass_num);

            temp_dir = "c:\\temp";
            textBoxWorkDir.Text = temp_dir;

            BuildCommandLine();
            DisplayCommandLine();

            // disable some control elements
            if (memory_test_only == 0)
            {
                radioButtonFileIoTesting.Checked = true;
                radioButtonMemoryOnlyTesting.Checked = false;
                checkBoxMemoryOnlyTest.Checked = true;
                EnableMemoryTestItems();
            }
            else
            {
                radioButtonFileIoTesting.Checked = false;
                radioButtonMemoryOnlyTesting.Checked = true;
                checkBoxMemoryOnlyTest.Checked = false;
                DisableMemoryTestItems();
            }

        }

        private void buttonExit_click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void buttonRun_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process p = new System.Diagnostics.Process();
            p.StartInfo.FileName = "cmd.exe";
            p.StartInfo.Arguments = "/k " + user_command_line;
            p.Start();
            //MessageBox.Show("Not implemented yet");
        }

        private void textBoxCommandLine_TextChanged(object sender, EventArgs e)
        {
            user_command_line = textBoxCommandLine.Text;
        }

        private void textBoxBufSize_TextChanged(object sender, EventArgs e)
        {
            string temp_str = textBoxBufSize.Text;
            if (temp_str.Length > 0)
            {
                try
                {
                    buf_size = Convert.ToUInt32(temp_str);
                }
                catch (System.Exception err)
                {
                    Unreferenced.Parameter(err);
                    buf_size = 4;
                }
                if (buf_size < 1) buf_size = 4;
                textBoxBufSize.Text = Convert.ToString(buf_size);
                BuildCommandLine();
                DisplayCommandLine();
            }
        }        

        private void textBoxWorkDir_TextChanged(object sender, EventArgs e)
        {
            temp_dir = textBoxWorkDir.Text;
            BuildCommandLine();
            DisplayCommandLine();
        } 

        private void textBoxFileSize_TextChanged(object sender, EventArgs e)
        {
            string temp_str = textBoxFileSize.Text;
            try
            {
                file_size = Convert.ToUInt32(temp_str);
            }
            catch (System.Exception err)
            {
                Unreferenced.Parameter(err);
                file_size = 16;
            }
            textBoxFileSize.Text = Convert.ToString(file_size);
            BuildCommandLine();
            DisplayCommandLine();
        } 

        private void textBoxFilesNum_TextChanged(object sender, EventArgs e)
        {
            string temp_str = textBoxFilesNum.Text;
            try
            {
                buffers_num = Convert.ToInt16(temp_str);
            }
            catch (System.Exception err)
            {
                Unreferenced.Parameter(err);
                buffers_num = 1;
            }
            if (buffers_num > 100) buffers_num = 100;
            if (buffers_num < 1)   buffers_num = 1;
            textBoxFilesNum.Text = Convert.ToString(buffers_num);
            BuildCommandLine();
            DisplayCommandLine();            
        } 

        private void textBoxPassesNum_TextChanged(object sender, EventArgs e)
        {
            string temp_str = textBoxPassesNum.Text;
            try
            {
                pass_num = Convert.ToUInt32(temp_str);
            }
            catch (System.Exception err)
            {
                Unreferenced.Parameter(err);
                pass_num = 1;
            }
            textBoxPassesNum.Text = Convert.ToString(pass_num);
            BuildCommandLine();
            DisplayCommandLine();
        }

        private void textBoxThreadsNum_TextChanged(object sender, EventArgs e)
        {
            string temp_str = textBoxThreadsNum.Text;
            try
            {
                threads_num = Convert.ToInt16(temp_str);
            }
            catch (System.Exception err)
            {
                Unreferenced.Parameter(err);
                threads_num = 1;
            }
            if (threads_num > 500) threads_num = 500;
            if (threads_num < 1)   threads_num = 1;
            textBoxThreadsNum.Text = Convert.ToString(threads_num);
            BuildCommandLine();
            DisplayCommandLine();
        }

        private void textBoxAutoBufSize_TextChanged(object sender, EventArgs e)
        {
            string temp_str = textBoxAutoBufSize.Text;
            try
            {
                auto_buf_size = Convert.ToUInt32(temp_str);
            }
            catch (System.Exception err)
            {
                Unreferenced.Parameter(err);
                auto_buf_size = 1000;
            }
            textBoxAutoBufSize.Text = Convert.ToString(auto_buf_size);
            BuildCommandLine();
            DisplayCommandLine();
        }

        private void checkBoxFillRandom_CheckStateChanged(object sender, EventArgs e)
        {
            if (checkBoxFillRandom.Checked == true)  random_fill = 1;
            if (checkBoxFillRandom.Checked == false) random_fill = 0;
            BuildCommandLine();
            DisplayCommandLine();
        }

        private void radioButtonFileIoTesting_CheckedChanged(object sender, EventArgs e)
        {
            //if (memory_test_only == 0)
            if (radioButtonFileIoTesting.Checked == false)
            {
                memory_test_only = 1;
                EnableMemoryTestItems();
            }
            else
            {
                memory_test_only = 0;
                DisableMemoryTestItems();
            }
            BuildCommandLine();
            DisplayCommandLine();
        }

        private void radioButtonMemoryOnlyTesting_CheckedChanged(object sender, EventArgs e)
        {
            // nothing
        }

        private void checkBoxLoopForever_CheckStateChanged(object sender, EventArgs e)
        {
            if (checkBoxLoopForever.Checked == true) loop_forever = 1;
            if (checkBoxLoopForever.Checked == false) loop_forever = 0;
            BuildCommandLine();
            DisplayCommandLine();
        }

        private void checkBoxTimeProfiling_CheckStateChanged(object sender, EventArgs e)
        {
            if (checkBoxTimeProfiling.Checked == true)  time_profiling = 1;
            if (checkBoxTimeProfiling.Checked == false) time_profiling = 0;
            BuildCommandLine();
            DisplayCommandLine();
        }

        private void checkBoxVerbose_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBoxVerbose.Checked == true)  verbose = 1;
            if (checkBoxVerbose.Checked == false) verbose = 0;
            BuildCommandLine();
            DisplayCommandLine();
        }

        private void checkBoxAutoAllMemoryTesting_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBoxAutoAllMemoryTesting.Checked == true) autodetect_memory_test = 1;
            if (checkBoxAutoAllMemoryTesting.Checked == false) autodetect_memory_test = 0;
            BuildCommandLine();
            DisplayCommandLine();
        }

        private void checkBoxMemoryOnlyTest_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBoxMemoryOnlyTest.Checked == true)  memory_test_only = 1;
            if (checkBoxMemoryOnlyTest.Checked == false) memory_test_only = 0;
            BuildCommandLine();
            DisplayCommandLine();
        }

        private void buttonAbout_Click(object sender, EventArgs e)
        {
            MessageBox.Show("FTest3 GUI interface C#\r\nCopyright (c) 2016\r\nDmitry Stefankov", "About", MessageBoxButtons.OK);
        }

        private void buttonHelp_Click(object sender, EventArgs e)
        {
            Assembly assembly = Assembly.GetExecutingAssembly();
            //foreach (string s in assembly.GetManifestResourceNames())
            //    System.Diagnostics.Debug.WriteLine(s);
            //Stream fileStream = assembly.GetManifestResourceStream("DefaultNameSpaceOfAssembly.ftest3.help");
            Stream fileStream = assembly.GetManifestResourceStream("Win32_winforms_csharp.Resources.ftest3.help");
            if (fileStream == null) System.Diagnostics.Debug.WriteLine("Resource not found");
            if (fileStream != null)
            {
                using (var reader = new StreamReader(fileStream))
                {
                    string text = reader.ReadToEnd();
                    TextBox TextBox1 = new TextBox();
                    TextBox1.Width = 400; 
                    TextBox1.Height = 500;
                    TextBox1.Multiline = true;
                    TextBox1.ReadOnly = true;
                    TextBox1.Enabled = true;
                    TextBox1.BackColor = Color.Blue;
                    TextBox1.ForeColor = Color.White;
                    TextBox1.BorderStyle = BorderStyle.Fixed3D;
                    TextBox1.Text = text;
                    TextBox1.Visible = true;
                    TextBox1.ScrollBars = ScrollBars.Both;
                    TextBox1.WordWrap = true;
                    TextBox1.AcceptsReturn = true;
                    TextBox1.Location = new System.Drawing.Point(0, 0);

                    Form dlg1 = new Form();
                    dlg1.Text = "FTest3 console help";
                    dlg1.AutoSize = true;
                    dlg1.Controls.Add(TextBox1);
                    dlg1.ShowDialog();

                }
            }
        }

    }

    public class Unreferenced
    {
        [System.Diagnostics.Conditional("DEBUG")]
        static public void Parameter(params object[] o)
        {
            return;
        }
    }
    

}

