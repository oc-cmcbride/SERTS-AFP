Imports System.Threading
Imports System.Windows.Forms.VisualStyles.VisualStyleElement.Window

Public Class Form1
    Delegate Sub ListBoxDelegate(ByVal command As Integer, ByVal myStr As String)
    Dim ListBoxDel As New ListBoxDelegate(AddressOf ListBoxDelMethod)

    Dim PC_Comm As New Thread(AddressOf PC_Comm_method)
    Dim ShowFiles As Integer = 1
    Dim StartFileList As Integer = 2
    Dim EndFileList As Integer = 3
    Dim PlayFile As Integer = 4
    Dim PauseFile As Integer = 5
    Dim StopFile As Integer = 6

    Dim ShowFilesStr As String = "1"
    Dim StartFileListStr As String = "2"
    Dim EndFileListStr As String = "3"
    Dim PlayFileStr As String = "4"
    Dim PauseFileStr As String = "5"
    Dim StopFileStr As String = "6"

    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load

        ' Get the list of available com ports and put them in the list box
        ComResultTextBox.Text = "Com port closed"
        For Each sp As String In My.Computer.Ports.SerialPortNames
            ComListBox.Items.Add(sp)
        Next
        ResumeButton.Hide()
        PauseButton.Hide()
        StopButton.Hide()

        ' Make this a background thread so it automatically
        ' aborts when the main program stops.
        PC_Comm.IsBackground = True
        ' Set the thread priority to lowest
        PC_Comm.Priority = ThreadPriority.Lowest
        ' Start the thread
        PC_Comm.Start()
    End Sub

    Public Sub ListBoxDelMethod(ByVal command As Integer, ByVal myStr As String)

        If command = StartFileList Then
            FileListBox.Items.Clear()
        ElseIf command = EndFileList Then
            FileListBox.Items.Add(myStr)
        End If
    End Sub

    Private Sub PC_Comm_method()
        Dim str As String
        While 1
            If SerialPort1.IsOpen Then
                Try
                    str = SerialPort1.ReadLine() ' Wait for start string
                Catch ex As Exception
                End Try

                'String.Compare return values:
                ' Less than zero: strA precedes strB in the sort order.
                'Zero" strA occurs in the same position as strB in the sort order.
                'Greater than zero: strA follows strB in the sort order.
                If Not String.Compare(str, StartFileListStr) Then
                    ' Received a StartFileList response
                    ' clear the list
                    ' Use the delegate to access the ListBox
                    FileListBox.Invoke(ListBoxDel, StartFileList, "")
                    ' get next string
                    Try
                        str = SerialPort1.ReadLine() ' read file name
                    Catch ex As Exception
                    End Try
                    While String.Compare(str, EndFileListStr)
                        ' The read string is a file name and not the EndFileList
                        FileListBox.Invoke(ListBoxDel, EndFileList, str)
                        Try
                            str = SerialPort1.ReadLine() ' read file name
                        Catch ex As Exception
                        End Try
                    End While
                    ' While loop ends when a 3 is received
                End If
            Else
                Threading.Thread.Sleep(500)
            End If
        End While
    End Sub
    Private Sub Connect_Click(sender As Object, e As EventArgs) Handles Connect.Click

        ' Connect to a selected serial port
        If SerialPort1.IsOpen = False Then
            ' a serial port in not already open
            If ComListBox.SelectedItem <> Nothing Then
                ' an item in the listbox has been selected
                SerialPort1.PortName = ComListBox.SelectedItem

                Try
                    'try to open the selected serial port
                    SerialPort1.Open()
                Catch
                    Debug.WriteLine("Failed to open serial port")
                End Try
                If SerialPort1.IsOpen Then
                    ' The serial port was opened
                    ComResultTextBox.Text = "Com port " + SerialPort1.PortName + " connected"
                Else
                    ' The serial port open failed
                    ComResultTextBox.Text = "Com port " + SerialPort1.PortName + " not connected"
                End If
            Else
                ComResultTextBox.Text = "Nothing selected"
            End If
        Else
            ' A com port is already open
            ComResultTextBox.Text = "Com port " + SerialPort1.PortName + " already connected"
        End If

    End Sub

    Private Sub Disconnect_Click(sender As Object, e As EventArgs) Handles Disconnect.Click
        ' Close the serial port if it is open
        If SerialPort1.IsOpen Then
            SerialPort1.Close()
            ComResultTextBox.Text = "Com port closed"
        End If
    End Sub


    Private Sub Index_Click(sender As Object, e As EventArgs) Handles IndexButton.Click
        If SerialPort1.IsOpen Then
            ' Send Show_Files command
            SerialPort1.Write(ShowFilesStr, 0, 1)
        End If
    End Sub

    Private Sub Play_Click(sender As Object, e As EventArgs) Handles PlayButton.Click
        Dim b(1) As Byte
        b(0) = 0
        ' A value of negative one (-1) is returned if no item is selected
        If Not (FileListBox.SelectedIndex = -1) Then
            SerialPort1.Write(PlayFileStr, 0, 1)
            For index As Integer = 1 To 30
            Next
            SerialPort1.Write(FileListBox.SelectedItem)
            SerialPort1.Write(b, 0, 1)
        End If
        ResumeButton.Hide()
        PauseButton.Show()
        StopButton.Show()

    End Sub
    Private Sub Resume_Click(sender As Object, e As EventArgs) Handles ResumeButton.Click
        Dim b(1) As Byte
        b(0) = 0
        ' A value of negative one (-1) is returned if no item is selected
        If Not (FileListBox.SelectedIndex = -1) Then
            SerialPort1.Write(PlayFileStr, 0, 1)
            For index As Integer = 1 To 30
            Next
            SerialPort1.Write(FileListBox.SelectedItem)
            SerialPort1.Write(b, 0, 1)
        End If
        ResumeButton.Hide()
        PauseButton.Show()
        StopButton.Show()

    End Sub

    Private Sub Pause_Click(sender As Object, e As EventArgs) Handles PauseButton.Click
        If SerialPort1.IsOpen Then
            ' Send Show_Files command
            SerialPort1.Write(PauseFileStr, 0, 1)
        End If
        ResumeButton.Show()
        PauseButton.Hide()
        StopButton.Hide()
    End Sub

    Private Sub Stop_Click(sender As Object, e As EventArgs) Handles StopButton.Click
        If SerialPort1.IsOpen Then
            ' Send Show_Files command
            SerialPort1.Write(StopFileStr, 0, 1)
        End If
        PauseButton.Hide()
        StopButton.Hide()
    End Sub

End Class


