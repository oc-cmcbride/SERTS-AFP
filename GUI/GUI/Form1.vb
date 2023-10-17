Imports System.Threading

Public Class Form1


    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load


        ' Get the list of available com ports and put them in the list box
        ComResultTextBox.Text = "Com port closed"
        For Each sp As String In My.Computer.Ports.SerialPortNames
            ComListBox.Items.Add(sp)
        Next
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


End Class
