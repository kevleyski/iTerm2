//
//  VT100TerminalDelegate.h
//  iTerm
//
//  Created by George Nachman on 10/26/13.
//
//

#import <Foundation/Foundation.h>
#import "VT100InlineImageHelper.h"
#import "VT100Token.h"
#import "iTermPromise.h"

@class ParsedSSHOutput;
@class VT100SavedColorsSlot;
@class iTermKittyImageCommand;
@class iTermTokenExecutorUnpauser;

typedef NS_ENUM(NSInteger, MouseMode) {
    MOUSE_REPORTING_NONE = -1,
    MOUSE_REPORTING_NORMAL = 0,
    MOUSE_REPORTING_HIGHLIGHT = 1,
    MOUSE_REPORTING_BUTTON_MOTION = 2,
    MOUSE_REPORTING_ALL_MOTION = 3,
};

typedef NS_ENUM(NSInteger, VT100TerminalSemanticTextType) {
    kVT100TerminalSemanticTextTypeFilename = 1,
    kVT100TerminalSemanticTextTypeDirectory = 2,
    kVT100TerminalSemanticTextTypeProcessId = 3,

    kVT100TerminalSemanticTextTypeMax
};

typedef NS_ENUM(NSUInteger, VT100AttentionRequestType) {
    VT100AttentionRequestTypeStartBouncingDockIcon,
    VT100AttentionRequestTypeStopBouncingDockIcon,
    VT100AttentionRequestTypeBounceOnceDockIcon,
    VT100AttentionRequestTypeFireworks,
    VT100AttentionRequestTypeFlash
};

typedef NS_ENUM(int, VT100TerminalColorIndex) {
    VT100TerminalColorIndexText = -1,
    VT100TerminalColorIndexBackground = -2,
    VT100TerminalColorIndexCursor = -3,
    VT100TerminalColorIndexSelectionBackground = -4,
    VT100TerminalColorIndexSelectionForeground = -5,

    VT100TerminalColorIndexFirst8BitColorIndex = 0,
    VT100TerminalColorIndexLast8BitColorIndex = 255
};

typedef NS_ENUM(NSUInteger, VT100TerminalProtectedMode) {
    VT100TerminalProtectedModeNone = 0,
    VT100TerminalProtectedModeISO = 1,
    VT100TerminalProtectedModeDEC = 2
};

@protocol VT100TerminalDelegate <NSObject>
// Append a string at the cursor's position and advance the cursor, scrolling if necessary.
- (void)terminalAppendString:(NSString *)string;
- (void)terminalAppendAsciiData:(AsciiData *)asciiData;

// Play/display the bell.
- (void)terminalRingBell;

// Move the cursor back, possibly wrapping around to the previous line.
- (void)terminalBackspace;

// Move the cursor to the next tab stop, erasing until that point.
- (void)terminalAppendTabAtCursor:(BOOL)setBackgroundColors;

// Move the cursor down, scrolling if necessary.
- (void)terminalLineFeed;

// Move the cursor left one place.
- (void)terminalCursorLeft:(int)n;

// Move the cursor down one row.
- (void)terminalCursorDown:(int)n andToStartOfLine:(BOOL)toStart;

// Move the cursor right one place.
- (void)terminalCursorRight:(int)n;

// Move the cursor up one row.
- (void)terminalCursorUp:(int)n andToStartOfLine:(BOOL)toStart;

// Move the cursor to a 1-based coordinate.
- (void)terminalMoveCursorToX:(int)x y:(int)y;

// Returns if it's safe to send reports.
- (BOOL)terminalShouldSendReport;

- (void)terminalReportVariableNamed:(NSString *)variable;

// Sends a report.
- (void)terminalSendReport:(NSData *)report;

// Replaces the screen contents with a test pattern.
- (void)terminalShowTestPattern;

// Returns the cursor's position relative to the scroll region's origin. 1-based.
- (int)terminalRelativeCursorX;

// Returns the cursor's position relative to the scroll region's origin. 1-based.
- (int)terminalRelativeCursorY;

// Set the top/bottom scroll region. 0-based. Inclusive.
- (void)terminalSetScrollRegionTop:(int)top bottom:(int)bottom;

// Returns the scroll region, or the whole screen if none is set.
- (VT100GridRect)terminalScrollRegion;

// Sums the visible characters in the given rectangle onscreen.
- (int)terminalChecksumInRectangle:(VT100GridRect)rect;
- (NSArray<NSString *> *)terminalSGRCodesInRectangle:(VT100GridRect)rect;

// Erase all characters before the cursor and/or after the cursor.
- (void)terminalEraseInDisplayBeforeCursor:(BOOL)before afterCursor:(BOOL)after;

// Erase all lines before/after the cursor. If erasing both, the screen is copied into the
// scrollback buffer.
- (void)terminalEraseLineBeforeCursor:(BOOL)before afterCursor:(BOOL)after;

// Set a tabstop at the current cursor column.
- (void)terminalSetTabStopAtCursor;

// Move the cursor to the left margin.
- (void)terminalCarriageReturn;

// Scroll up one line.
- (void)terminalReverseIndex;

// Move cursor to the right/left one column, scrolling content left/right if at margin.
- (void)terminalForwardIndex;
- (void)terminalBackIndex;

// Clear the screen, preserving the cursor's line.
- (void)terminalResetPreservingPrompt:(BOOL)preservePrompt modifyContent:(BOOL)modifyContent;

// Changes the cursor type.
- (void)terminalSetCursorType:(ITermCursorType)cursorType;

// Changes whether the cursor blinks.
- (void)terminalSetCursorBlinking:(BOOL)blinking;
- (iTermPromise<NSNumber *> *)terminalCursorIsBlinkingPromise;

// Reset type and blink to default
- (void)terminalResetCursorTypeAndBlink;

// Returns the current cursor style as a DECSCUSR param.
- (void)terminalGetCursorInfoWithCompletion:(void (^)(ITermCursorType type, BOOL blinking))completion;

// Sets the left/right scroll region.
- (void)terminalSetLeftMargin:(int)scrollLeft rightMargin:(int)scrollRight;

// Sets whether one charset is in linedrawing mode.
- (void)terminalSetCharset:(int)charset toLineDrawingMode:(BOOL)lineDrawingMode;
- (BOOL)terminalLineDrawingFlagForCharset:(int)index;

// Remove all tab stops.
- (void)terminalRemoveTabStops;

// Remove the tab stop at the cursor's current column.
- (void)terminalRemoveTabStopAtCursor;

// Sorted tab stops, 1-based.
- (NSArray<NSNumber *> *)terminalTabStops;

// Tab stops, 1 based. All numbers will be positive.
- (void)terminalSetTabStops:(NSArray<NSNumber *> *)tabStops;

// Tries to resize the screen to |width|.
- (void)terminalSetWidth:(int)width
          preserveScreen:(BOOL)preserveScreen
           updateRegions:(BOOL)updateRegions
            moveCursorTo:(VT100GridCoord)newCursorCoord
              completion:(void (^)(void))completion;

// Moves cursor to previous tab stop.
- (void)terminalBackTab:(int)n;

// Sets the cursor's x coordinate. 1-based.
- (void)terminalSetCursorX:(int)x;
- (void)terminalAdvanceCursorPastLastColumn;

// Sets the cursor's y coordinate. 1-based.
- (void)terminalSetCursorY:(int)y;

// Erases some number of characters after the cursor, replacing them with blanks.
- (void)terminalEraseCharactersAfterCursor:(int)j;

// Send the current print buffer to the printer.
- (void)terminalPrintBuffer;

// Future input (linefeeds, carriage returns, and appended strings) should be saved for printing and not displayed.
- (void)terminalBeginRedirectingToPrintBuffer;

// Send the current screen contents to the printer.
- (void)terminalPrintScreen;

// Sets the window's title.
- (void)terminalSetWindowTitle:(NSString *)title;

// Sets the icon's title.
- (void)terminalSetIconTitle:(NSString *)title;
- (void)terminalSetSubtitle:(NSString *)subtitle;

// Pastes a string to the shell.
- (void)terminalCopyStringToPasteboard:(NSString *)string;
- (void)terminalReportPasteboard:(NSString *)pasteboard;

// Inserts |n| blank chars after the cursor, moving chars to the right of them over.
- (void)terminalInsertEmptyCharsAtCursor:(int)n;

// Shift line left by `n` chars.
- (void)terminalShiftLeft:(int)n;
- (void)terminalShiftRight:(int)n;

// Inserts |n| blank lines after the cursor, moving lines below them down.
- (void)terminalInsertBlankLinesAfterCursor:(int)n;

// Deletes |n| characters after the cursor, moving later chars left.
- (void)terminalDeleteCharactersAtCursor:(int)n;

// Deletes |n| lines after the cursor, moving later lines up.
- (void)terminalDeleteLinesAtCursor:(int)n;

// Tries to resize the screen to |rows| by |columns|.
- (void)terminalSetRows:(int)rows andColumns:(int)columns;

// Tries to resize the window to the given pixel size.
- (void)terminalSetPixelWidth:(int)width height:(int)height;

// Tries to move the window's top left coordinate to the given point.
- (void)terminalMoveWindowTopLeftPointTo:(NSPoint)point;

// Either miniaturizes or unminiaturizes, depending on |mini|.
- (void)terminalMiniaturize:(BOOL)mini;

// Either raises or iconifies, depending on |raise|.
- (void)terminalRaise:(BOOL)raise;

// Scroll the screen's scroll region up by |n| lines.
- (void)terminalScrollUp:(int)n;

// Scroll the screen's scroll region down by |n| lines.
- (void)terminalScrollDown:(int)n;

// Returns if the window is miniaturized.
- (BOOL)terminalWindowIsMiniaturized;

// Returns the top-left pixel coordinate of the window.
- (NSPoint)terminalWindowTopLeftPixelCoordinate;

// Returns the size of the window in pixels.
- (int)terminalWindowWidthInPixels;
- (int)terminalWindowHeightInPixels;

// Returns the size of the screen the window is on in cells.
- (int)terminalScreenHeightInCells;
- (int)terminalScreenWidthInCells;

// Returns the current icon (tab)/window title.
- (void)terminalReportIconTitle;
- (void)terminalReportWindowTitle;

// Saves the current window/icon (depending on isWindow) title in a stack.
- (void)terminalPushCurrentTitleForWindow:(BOOL)isWindow;

// Restores the window/icon (depending on isWindow) title from a stack.
- (void)terminalPopCurrentTitleForWindow:(BOOL)isWindow;

// Posts a message to Notification center.
- (void)terminalPostUserNotification:(NSString *)message;

// Posts a rich notification message to Notification center.
- (void)terminalPostUserNotification:(NSString *)message rich:(BOOL)rich;

// Enters Tmux mode.
- (void)terminalStartTmuxModeWithDCSIdentifier:(NSString *)dcsID;

// Handles input during tmux mode. A single line of input will be in the token's string.
- (void)terminalHandleTmuxInput:(VT100Token *)token;
- (void)terminalDidTransitionOutOfTmuxMode;

- (VT100GridSize)terminalSizeInCells;

// Returns the size of a single cell. May contain non-integer values.
- (NSSize)terminalCellSizeInPoints:(double *)scale;

// Called when the mouse reporting mode changes.
- (void)terminalMouseModeDidChangeTo:(MouseMode)mouseMode;

// Called when the terminal needs to be redrawn.
- (void)terminalNeedsRedraw;

// Sets whether the left/right scroll region should be used.
- (void)terminalSetUseColumnScrollRegion:(BOOL)use;
- (BOOL)terminalUseColumnScrollRegion;

// Switches the currently visible buffer.
- (void)terminalShowAltBuffer;
- (BOOL)terminalIsShowingAltBuffer;

// Show main (primary) buffer. Does nothing if already on the alt grid.
- (void)terminalShowPrimaryBuffer;

// Clears the screen, preserving the wrapped line the cursor is on.
- (void)terminalClearScreen;

// Erase scrollback history, leave screen alone.
- (void)terminalClearScrollbackBuffer;

// Saves the current scroll position in the window.
- (void)terminalSaveScrollPositionWithArgument:(NSString *)argument;

// Make the current terminal visible and give it keyboard focus.
- (void)terminalStealFocus;

// Set the proxy icon of current session window.
- (void)terminalSetProxyIcon:(NSString *)value;

// Erase the screen (preserving the line the cursor is on) and the scrollback buffer.
- (void)terminalClearBuffer;

// Called when the current directory may have changed.
- (void)terminalCurrentDirectoryDidChangeTo:(NSString *)value;

// Sets the username@hostname or hostname of the current cursor location.
- (void)terminalSetRemoteHost:(NSString *)remoteHost;
- (void)terminalSetWorkingDirectoryURL:(NSString *)URL;

// The profile should change to one with the name |value|.
- (void)terminalProfileShouldChangeTo:(NSString *)value;

// Set a note. Value is message or length|message or x|y|length|message
- (void)terminalAddNote:(NSString *)value show:(BOOL)show;

// Sets the current pasteboard. Legal values are ruler, find, and font. Other values, including
// empty string, are treated as the default pasteboard.
- (void)terminalSetPasteboard:(NSString *)value;
- (void)terminalCopyBufferToPasteboard;
- (void)terminalAppendDataToPasteboard:(NSData *)data;

// Download of a base64-encoded file
// nil = name unknown, -1 = size unknown. Return YES to accept it.
- (void)terminalWillReceiveFileNamed:(NSString *)name
                              ofSize:(NSInteger)size
                          completion:(void (^)(BOOL ok))completion;

- (void)terminalWillReceiveInlineFileNamed:(NSString *)name
                                    ofSize:(NSInteger)size
                                     width:(int)width
                                     units:(VT100TerminalUnits)widthUnits
                                    height:(int)height
                                     units:(VT100TerminalUnits)heightUnits
                       preserveAspectRatio:(BOOL)preserveAspectRatio
                                     inset:(NSEdgeInsets)inset
                                      type:(NSString *)type
                                 forceWide:(BOOL)forceWide
                                completion:(void (^)(BOOL ok))completion;

// Download completed normally
- (void)terminalDidFinishReceivingFile;

// Got another chunk of base-64 encoded data for the current download.
// Preceded by terminalWillReceiveFileNamed:size:.
- (void)terminalDidReceiveBase64FileData:(NSString *)data;

// Got bogus data, abort download.
// Preceded by terminalWillReceiveFileNamed:size: and possibly some
// terminalDidReceiveBase64FileData: calls.
- (void)terminalFileReceiptEndedUnexpectedly;

// Begin sending a file upload (base64 filename + newline + base64 file + newline)
// args are "multiple" (meaning multiple files are allowed) or "single".
- (void)terminalRequestUpload:(NSString *)args;

// Signal the user that the terminal wants attention.
- (void)terminalRequestAttention:(VT100AttentionRequestType)request;

// Disinter a buried session
- (void)terminalDisinterSession;

// Set various colors.
- (void)terminalSetForegroundColor:(NSColor *)color;
- (void)terminalSetBackgroundColor:(NSColor *)color;
- (void)terminalSetBoldColor:(NSColor *)color;
- (void)terminalSetSelectionColor:(NSColor *)color;
- (void)terminalSetSelectedTextColor:(NSColor *)color;
- (void)terminalSetCursorColor:(NSColor *)color;
- (void)terminalSetCursorTextColor:(NSColor *)color;
- (void)terminalSetColorTableEntryAtIndex:(VT100TerminalColorIndex)n color:(NSColor *)color;
- (void)terminalResetColor:(VT100TerminalColorIndex)color;

// Change the color tint of the current tab.
- (void)terminalSetCurrentTabColor:(NSColor *)color;
- (void)terminalSetTabColorRedComponentTo:(CGFloat)color;
- (void)terminalSetTabColorGreenComponentTo:(CGFloat)color;
- (void)terminalSetTabColorBlueComponentTo:(CGFloat)color;

- (NSColor *)terminalColorForIndex:(VT100TerminalColorIndex)index;

// Returns the current cursor position.
- (int)terminalCursorX;
- (int)terminalCursorY;
- (BOOL)terminalWillAutoWrap;  // Is cursor just past the last column?

// Shows/hides the cursor.
- (void)terminalSetCursorVisible:(BOOL)visible;

- (void)terminalSetHighlightCursorLine:(BOOL)highlight;

// FinalTerm features
- (void)terminalPromptDidStart:(BOOL)wasInCommand;
- (void)terminalCommandDidStart;
- (void)terminalCommandDidEnd;
- (void)terminalSemanticTextDidStartOfType:(VT100TerminalSemanticTextType)type;
- (void)terminalSemanticTextDidEndOfType:(VT100TerminalSemanticTextType)type;
- (void)terminalProgressAt:(double)fraction label:(NSString *)label;
- (void)terminalProgressDidFinish;
- (void)terminalReturnCodeOfLastCommandWas:(int)returnCode;
- (void)terminalFinalTermCommand:(NSArray *)argv;
- (void)terminalSetShellIntegrationVersion:(NSString *)version;
- (void)terminalAbortCommand;

// Flag changes
- (void)terminalWraparoundModeDidChangeTo:(BOOL)newValue;
- (void)terminalTypeDidChange;
- (void)terminalInsertModeDidChangeTo:(BOOL)newValue;

- (NSString *)terminalProfileName;

- (void)terminalSetBackgroundImageFile:(NSString *)filename;
- (void)terminalSetBadgeFormat:(NSString *)badge;
- (void)terminalSetUserVar:(NSString *)kvp;

- (BOOL)terminalFocusReportingAllowed;
- (BOOL)terminalCursorVisible;
- (BOOL)terminalAllowAlternateMouseScroll;

- (BOOL)terminalIsTrusted;
- (BOOL)terminalCanUseDECRQCRA;

- (void)terminalSetUnicodeVersion:(NSInteger)unicodeVersion;
- (void)terminalSetColorNamed:(NSString *)name to:(NSString *)value;
- (NSInteger)terminalUnicodeVersion;

// Touch bar
- (void)terminalSetLabel:(NSString *)label forKey:(NSString *)keyName;
- (void)terminalPushKeyLabels:(NSString *)value;
- (void)terminalPopKeyLabels:(NSString *)value;

// Copy to pasteboard
- (void)terminalBeginCopyToPasteboard;
- (void)terminalDidReceiveBase64PasteboardString:(NSString *)string;
- (void)terminalDidFinishReceivingPasteboard;
- (void)terminalPasteboardReceiptEndedUnexpectedly;

// Links
- (void)terminalWillEndLinkWithURL:(iTermURL *)url;
- (void)terminalWillStartLinkWithURL:(iTermURL *)url;

// Custom escape sequences
- (void)terminalCustomEscapeSequenceWithParameters:(NSDictionary<NSString *, NSString *> *)parameters
                                           payload:(NSString *)payload;

- (void)terminalRepeatPreviousCharacter:(int)times;

- (void)terminalSynchronizedUpdate:(BOOL)begin;

- (void)terminalReportFocusWillChangeTo:(BOOL)reportFocus;
- (void)terminalPasteBracketingWillChangeTo:(BOOL)bracket;
- (void)terminalSoftAlternateScreenModeDidChange;

- (void)terminalReportKeyUpDidChange:(BOOL)reportKeyUp;

- (void)terminalAppendSixelData:(NSData *)sixelData;

- (void)terminalDidChangeSendModifiers;
- (void)terminalKeyReportingFlagsDidChange;
- (void)terminalClearCapturedOutput;

- (BOOL)terminalIsInAlternateScreenMode;

- (iTermPromise<NSString *> *)terminalStringForKeypressWithCode:(unsigned short)keycode
                                                          flags:(NSEventModifierFlags)flags
                                                     characters:(NSString *)characters
                                    charactersIgnoringModifiers:(NSString *)charactersIgnoringModifiers;
- (void)terminalApplicationKeypadModeDidChange:(BOOL)mode;
- (NSString *)terminalTopBottomRegionString;
- (NSString *)terminalLeftRightRegionString;

- (VT100SavedColorsSlot *)terminalSavedColorsSlot;
- (void)terminalRestoreColorsFromSlot:(VT100SavedColorsSlot *)slot;
- (int)terminalMaximumTheoreticalImageDimension;
- (void)terminalInsertColumns:(int)n;
- (void)terminalDeleteColumns:(int)n;
- (void)terminalSetAttribute:(int)sgrAttribute inRect:(VT100GridRect)rect;
- (void)terminalToggleAttribute:(int)sgrAttribute inRect:(VT100GridRect)rect;
- (void)terminalCopyFrom:(VT100GridRect)source to:(VT100GridCoord)dest;
- (void)terminalFillRectangle:(VT100GridRect)rect withCharacter:(unichar)ch;
- (void)terminalEraseRectangle:(VT100GridRect)rect;
- (void)terminalSelectiveEraseRectangle:(VT100GridRect)rect;
- (void)terminalSelectiveEraseInDisplay:(int)mode;
- (void)terminalSelectiveEraseInLine:(int)mode;
- (void)terminalProtectedModeDidChangeTo:(VT100TerminalProtectedMode)mode;
- (VT100TerminalProtectedMode)terminalProtectedMode;

- (dispatch_queue_t)terminalQueue;
- (iTermTokenExecutorUnpauser *)terminalPause;
- (void)terminalSendCapabilitiesReport;
- (void)terminalDidHookSSHConductorWithParams:(NSString *)token;
- (void)terminalDidReadSSHConductorLine:(NSString *)string depth:(int)depth;
- (void)terminalDidUnhookSSHConductor;
- (void)terminalDidBeginSSHConductorCommandWithIdentifier:(NSString *)identifier
                                                    depth:(int)depth;
- (void)terminalDidEndSSHConductorCommandWithIdentifier:(NSString *)identifier
                                                   type:(NSString *)type
                                                 status:(uint8_t)status
                                                  depth:(int)depth;
- (void)terminalHandleSSHSideChannelOutput:(NSString *)string
                                       pid:(int32_t)pid
                                   channel:(uint8_t)channel
                                     depth:(int)depth;
- (void)terminalDidReadRawSSHData:(NSData *)data
                              pid:(int)pid
                          channel:(int)channel;

- (void)terminalHandleSSHTerminatePID:(int)pid withCode:(int)code depth:(int)depth;
- (void)terminalUpdateEnv:(NSString *)value;
- (void)terminalBeginSSHIntegeration:(NSString *)args;
- (void)terminalSendConductor:(NSString *)args;
- (void)terminalEndSSH:(NSString *)uniqueID;
- (void)terminalBeginFramerRecoveryForChildOfConductorAtDepth:(int)parentDepth;
- (void)terminalHandleFramerRecoveryString:(NSString *)string;
- (void)terminalDidResynchronizeSSH;

- (void)terminalDidExecuteToken:(VT100Token *)token;
- (void)terminal:(VT100Terminal *)terminal willExecuteToken:(VT100Token *)token defaultChar:(const screen_char_t *)defaultChar encoding:(NSStringEncoding)encoding;
- (void)terminalOpenURL:(NSURL *)url;
- (void)terminalBlock:(NSString *)blockID start:(BOOL)start type:(NSString *)type render:(BOOL)render;
- (void)terminalInsertCopyButtonForBlock:(NSString *)blockID;
- (void)terminalSetPointerShape:(NSString *)pointerShape;
- (void)terminalDidReceiveKittyImageCommand:(iTermKittyImageCommand *)kittyImageCommand;
- (void)terminalStartWrappedCommand:(NSString *)command channel:(NSString *)uid;

@end
