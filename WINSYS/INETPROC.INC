		// these are the heart of the internet client side [MDB]
		case MW_DISCONNECTED:
			// get rid of our chat window
			DestroyWindow(hwndChat);
			closesocket(MySock.sock);

			// Let's tell someone
			MessageBox(hwnd, "Connection dropped", NULL, MB_OK);
			break;

		case MW_DATAREADY:
			// first check to see if the connection was closed
			if(LOWORD(wParam) == FD_CLOSE)
			{
				// connection closed, kill our socket
				DestroyWindow(hwndChat);
				closesocket(MySock.sock);

				// Let's tell someone
				MessageBox(hwnd, "Connection dropped", NULL, MB_OK);
				return(0);
			}

			// receive the message
			if(!recvdatamessage(&MySock, &xferbuf))
			{
				return(0);
			}

			// We've got the whole message! Now switch on the command flag
			switch(xferbuf.hdr.command)
			{
				extern LONG	PlayerHeight;
				extern LONG	inetStartX, inetStartY, inetStartA;
				char tempbuf[255];
				int i;
				LONG tx, ty, tz, ta;
				LONG *lp;
				WORD tempHash;

				case COMMAND_DATA:
					// what command
					switch(*(xferbuf.data))
					{
						case IMOVE:
							// move command
							lp = (LONG *)xferbuf.data;
							lp++;
						    tempHash = (WORD)*lp;
							lp++;

						    // find the index
							for(i=0; i<MAX_PLAYERS; i++)
							{
								if(sPlayers[i].hash == tempHash)
									break;
							}

							// did we get a valid player position?
							if(i == MAX_PLAYERS)
								break;	// no so leave


							tx = *lp;
							lp++;
							ty = *lp;
							lp++;
							tz = *lp;
							lp++;
							ta = *lp;
							set_thing ((LONG)sPlayers[i].index, tx, ty, tz+PlayerHeight, ta);
							StartSequenceOnce ((LONG)sPlayers[i].index, WALKSEQ);

							break;
					}
					break;

				case PRIVATE_CHAT:
				case PUBLIC_CHAT:
					// put the text into the list box
					SendMessage(GetDlgItem(hwndChat, ID_CHAT_LIST), LB_ADDSTRING, 0, (LPARAM)&xferbuf.data);

					// get number of items in the list box, delete top entry if more than 30
					i = SendMessage(GetDlgItem(hwndChat, ID_CHAT_LIST), LB_GETCOUNT, (WPARAM)0, (LPARAM)0);
					if(i>=30)
						SendMessage(GetDlgItem(hwndChat, ID_CHAT_LIST), LB_DELETESTRING, (WPARAM)0, (LPARAM)0);
					break;

				case REGISTER_NAME:
					// Here we have a new player to add
					// name is in xferbuf.data[2]

					// find a player position
					for(i=0; i<MAX_PLAYERS; i++)
					{
						if(sPlayers[i].hash == 0)
							break;
					}

					// did we get a valid player position?
					if(i == MAX_PLAYERS)
						break;	// no so leave

					// grab the hash value from the data
					sPlayers[i].hash = *((WORD *)xferbuf.data);

					// start the player
					sPlayers[i].index = create_thing(17, inetStartX, inetStartY);
					add_thing(sPlayers[i].index,find_ssector(inetStartX,inetStartY));
					mythings[sPlayers[i].index].angle = inetStartA;

					// add the name to the name list & player array
					SendMessage(GetDlgItem(hwndChat, ID_NAMES), LB_ADDSTRING, 0, (LPARAM)&xferbuf.data[2]);
					strcpy(sPlayers[i].name, &xferbuf.data[2]);

					// print a message that so-and-so has entered the chat
					wsprintf(tempbuf, "%s has made the connection.", &xferbuf.data[2]);
					SendMessage(GetDlgItem(hwndChat, ID_CHAT_LIST), LB_ADDSTRING, 0, (LPARAM)tempbuf);

					break;

				case REREGISTER_NAME:
					// A player has left private chat
					// Here we remove the '*' from the name on the list
					// name to remove is in xferbuf.data
					wsprintf(tempbuf, "* %s", xferbuf.data);
					if((iIndex = SendMessage(GetDlgItem(hwndChat, ID_NAMES), LB_FINDSTRINGEXACT, (UINT)-1, (LPARAM)&tempbuf)) != LB_ERR)
					{
						// found the index to the item...delete it
						SendMessage(GetDlgItem(hwndChat, ID_NAMES), LB_DELETESTRING, iIndex, 0);

						// re-add the string without the '*'
						SendMessage(GetDlgItem(hwndChat, ID_NAMES), LB_ADDSTRING, 0, (LPARAM)&xferbuf.data);
					}

					break;

				case DEREGISTER_NAME:
					// Someone entered private chat
					// Here we add the '*' to the name on the list
					// name to remove is in xferbuf.data
					if((iIndex = SendMessage(GetDlgItem(hwndChat, ID_NAMES), LB_FINDSTRINGEXACT, (UINT)-1, (LPARAM)&xferbuf.data)) != LB_ERR)
					{
						// found the index to the item...delete it
						SendMessage(GetDlgItem(hwndChat, ID_NAMES), LB_DELETESTRING, iIndex, 0);

						// re-add the string with the '*'
						wsprintf(tempbuf, "* %s", xferbuf.data);
						SendMessage(GetDlgItem(hwndChat, ID_NAMES), LB_ADDSTRING, 0, (LPARAM)&tempbuf);
					}

					break;

				case CLOSE_NAME:
					// A player has left, delete the name from the list
					// name to remove is in xferbuf.data

					// find the player to delete
					for(i=0; i<MAX_PLAYERS; i++)
					{
						if(strcmp(sPlayers[i].name, xferbuf.data) == 0)
						{
							remove_thing(sPlayers[i].index);
							sPlayers[i].hash = 0;
							sPlayers[i].index = 0;
							sPlayers[i].name[0] = '\0';
							break;
						}
					}

					if((iIndex = SendMessage(GetDlgItem(hwndChat, ID_NAMES), LB_FINDSTRINGEXACT, (UINT)-1, (LPARAM)&xferbuf.data)) != LB_ERR)
					{
						// found the index to the item...delete it
						SendMessage(GetDlgItem(hwndChat, ID_NAMES), LB_DELETESTRING, iIndex, 0);

						// print a message that so-and-so has left the chat
						wsprintf(tempbuf, "%s has dropped out.", &xferbuf.data);
						SendMessage(GetDlgItem(hwndChat, ID_CHAT_LIST), LB_ADDSTRING, 0, (LPARAM)tempbuf);
					}

					// see if anyone is left
					if((iIndex = SendMessage(GetDlgItem(hwndChat, ID_NAMES), LB_GETCOUNT, 0, 0)) == 0)
					{
						// dim the buttons till someone is selected
						EnableWindow(GetDlgItem(hwndChat, ID_CHAT_MUTE), FALSE);
						EnableWindow(GetDlgItem(hwndChat, ID_CHAT_COMPLAIN), FALSE);
						EnableWindow(GetDlgItem(hwndChat, ID_CHAT_PRIVATE), FALSE);
					}
					break;

				case STOP_PRIVATE:
					iPrivate = FALSE;
					SetWindowText(GetDlgItem(hwndChat, ID_CHAT_PRIVATE), "Private Chat");
					wsprintf(outtext, "Private chat with %s ended.", peername);
					SetWindowText(GetDlgItem(hwndChat, ID_CHAT_HELP), outtext);
					break;

				case REQUEST_SESSION:
					// Someone is asking for a private chat
					wsprintf(outtext, "%s requests a private chat", xferbuf.data);

					// should we accept?
					if(MessageBox(hwnd, outtext, "Chat request", MB_OKCANCEL) == IDOK)
					{
						// save the name of our peer
						lstrcpy(peername, (char *)xferbuf.data);

						// we've got ourselves a private session! send response
						xferbuf.hdr.command = SESSION_REQUEST_RESPONSE;
						*(xferbuf.data) = 1;	// flag to say we accept
						xferbuf.hdr.length = HDRSIZE + 1;
						senddatamessage(MySock.sock, &xferbuf);

						// Set us up in the private sector
						iPrivate = TRUE;
						SetWindowText(GetDlgItem(hwndChat, ID_CHAT_PRIVATE), "Stop Private Chat");
						return(0);
					}
					else
					{
						// We're obviously to stuck up to accept a session
						// from this low-life... Send the denial message
						xferbuf.hdr.command = SESSION_REQUEST_RESPONSE;
						*(xferbuf.data) = 0;	// flag to deny session
						xferbuf.hdr.length = HDRSIZE + 1;
						senddatamessage(MySock.sock, &xferbuf);
						return(0);
					}
					break;

				case SESSION_REQUEST_RESPONSE:
					// someone responded to our session request!
					if(MySock.status != SOCKSTAT_REQSESSION)
						return(0);	// we didn't expect this...drop packet

					SetWindowText(GetDlgItem(hwndChat, ID_CHAT_HELP), "");
					if(*xferbuf.data == 1)
					{
						// Happy, Happy, Joy, Joy!  Accepted by our peer
						iPrivate = TRUE;
						SetWindowText(GetDlgItem(hwndChat, ID_CHAT_PRIVATE), "Stop Private Chat");
						return(0);
					}
					else
					{
						// Depression...nobody loves us
						iPrivate = FALSE;
						return(0);
					}
					break;

				default:
					return(0);	// This is unexpected...drop packet
			}

			break;
