//
//  ViewController.swift
//  MSLD
//
//  Created by junsang on 2021/01/09.
//

import UIKit
import AVFoundation

class ViewController: UIViewController, AVCaptureVideoDataOutputSampleBufferDelegate {
    
    var preview: UIImageView!
    private var captureSession: AVCaptureSession = AVCaptureSession()
    private let videoDataOutput = AVCaptureVideoDataOutput()
    
    private var fps: UILabel!
    private var startTime: CFTimeInterval!
    
    // UI interface
    let gap : CGFloat = UIScreen.main.bounds.size.height * 0.5 - 340

    enum DrawMode: String {
        case DEFAULT = "Default"
        case GRAY_SCALE = "Gray Scale"
        case CANNY_EDGE = "Canny Edge"
        case LINE_DETECT = "Line Detect"
    }
    
    var touchToSelectDrawMode:Int = 0
    var drawMode: DrawMode = .DEFAULT
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        self.preview = self.buildPreview()
        view.addSubview(preview)
        
        self.fps = self.buildFPSLabel()
        view.addSubview(self.fps)
        startTime = CACurrentMediaTime()
        
        let searchButton = self.buildsearchButton(position:CGPoint(x: self.view.frame.width * 0.5, y: gap + 640))
        view.addSubview(searchButton)
        
        let drawModeButton = self.buildDrawModeButton(position: CGPoint(x: (searchButton.layer.position.x + UIScreen.main.bounds.width) * 0.5 + 30, y: gap + 640.0))
        view.addSubview(drawModeButton)
        
        self.addCameraInput()
        self.getFrames()
        self.captureSession.startRunning()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
    
    private func addCameraInput() {
        guard let captureDevice = AVCaptureDevice.DiscoverySession(deviceTypes: [.builtInWideAngleCamera, .builtInDualCamera, .builtInTrueDepthCamera], mediaType: .video, position: .back).devices.first else {
            fatalError("No back camera device found, please make sure to run MSLD in an iOS device and not a simulator")
        }
        let cameraInput = try! AVCaptureDeviceInput(device:captureDevice)
        self.captureSession.addInput(cameraInput)
    }
    
    // MARK: AVCaptureVideoDataOutputSampleBufferDelegate
    func captureOutput(_ output: AVCaptureOutput, didOutput sampleBuffer: CMSampleBuffer, from connection: AVCaptureConnection) {
        guard let  imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer) else { return }
        CVPixelBufferLockBaseAddress(imageBuffer, CVPixelBufferLockFlags.readOnly)

        var bitmapInfo: UInt32 = CGBitmapInfo.byteOrder32Little.rawValue
        bitmapInfo |= CGImageAlphaInfo.premultipliedFirst.rawValue & CGBitmapInfo.alphaInfoMask.rawValue
        let context = CGContext(data: CVPixelBufferGetBaseAddress(imageBuffer),
                                width: CVPixelBufferGetWidth(imageBuffer),
                                height: CVPixelBufferGetHeight(imageBuffer),
                                bitsPerComponent: 8,
                                bytesPerRow: CVPixelBufferGetBytesPerRow(imageBuffer),
                                space: CGColorSpaceCreateDeviceRGB(),
                                bitmapInfo: bitmapInfo)
        guard let quartzImage = context?.makeImage() else { return }
        CVPixelBufferUnlockBaseAddress(imageBuffer, CVPixelBufferLockFlags.readOnly)
        let image = UIImage(cgImage: quartzImage)
        
        var resultImage: UIImage!
        switch self.touchToSelectDrawMode {
        case 0:
            self.drawMode = .DEFAULT
            resultImage = image
        case 1:
            self.drawMode = .GRAY_SCALE
            resultImage = OpenCVWrapper.convert(toGray: image)
        case 2:
            self.drawMode = .CANNY_EDGE
            resultImage = OpenCVWrapper.convert(toCannyEdge: image)
        case 3:
            self.drawMode = .LINE_DETECT
            resultImage = OpenCVWrapper.detectLine(image, isRandomColor: false, hasDescriptor: true)
        default:
            self.drawMode = .DEFAULT
            resultImage = image
        }
        
        DispatchQueue.main.async {
            self.preview.image = resultImage
            self.fps.text = String(format:"Draw mode: \(self.drawMode.rawValue)\nFPS:%.1f", 1 / Double(CACurrentMediaTime() - self.startTime))
            self.startTime = CACurrentMediaTime()
        }
    }
    
    // MARK: private
    private func getFrames() {
        // Any? https://zeddios.tistory.com/213
        // Any는 함수 타입을 포함하여 모든 타입의 인스턴스를 나타낼 수 있습니다.
        // AnyObject 모든 class 타입의 인스턴스를 나타낼 수 있습니다.
        
        // as: type casting https://gaki2745.github.io/swift/2019/10/02/Swift-Basic-04/
        // 타입 checking : is
        videoDataOutput.videoSettings = [(kCVPixelBufferPixelFormatTypeKey as NSString) : NSNumber(value: kCVPixelFormatType_32BGRA)] as [String : Any]
        
        videoDataOutput.alwaysDiscardsLateVideoFrames = true
        videoDataOutput.setSampleBufferDelegate(self, queue: DispatchQueue(label:"camera.frame.processing.queue"))
        self.captureSession.addOutput(videoDataOutput)
        
        // if 문에서의 , https://dev200ok.blogspot.com/2020/09/swift-if.html
        // if let result = result && result { print("true") } <- error
        // if let 으로 옵셔널을 제거한 뒤 사용 가능 -> comma(,)를 이용해 코드가능
        // let result: Bool? = false
        // if let result = result, result { print("true") } else { print("false") }
        // if let result = result && result { : 하나의 expression
        // if let result = result, result { // expression, expression 으로 이루어진 condition-list
        guard let connection = self.videoDataOutput.connection(with: AVMediaType.video), connection.isVideoOrientationSupported else { return }
        connection.videoOrientation = .portrait
    }
    
    // call by reference (preview: inout UIImageView)
    // call by reference :https://youngq.tistory.com/23
    private func buildPreview() -> UIImageView {
        let preview = UIImageView(frame: CGRect( x: 0,
                                                 y: 0,
                                                 width: UIScreen.main.bounds.size.width,
                                                 height: UIScreen.main.bounds.size.height))
        preview.contentMode = UIView.ContentMode.scaleAspectFit
        preview.layer.position = CGPoint(x: self.view.frame.width * 0.5,
                                         y: self.view.frame.height * 0.5)
//        preview.frame = CGRect(x:0, y:0, width: 480, height: 640)
        return preview
    }
    
    private func buildFPSLabel() -> UILabel {
        let fps = UILabel(frame: CGRect(x: 0, y: 0, width: self.view.frame.width * 0.5, height: 200))
        fps.backgroundColor = UIColor.clear
        fps.numberOfLines = 0
        fps.textAlignment = .left
        fps.text = "Default"
        fps.textColor = UIColor.green
        fps.layer.position = CGPoint(x: self.view.frame.width * 0.5, y: gap)
        return fps
    }
    
    private func buildsearchButton(position:CGPoint) -> UIButton {
        // 1. action
        // https://zeddios.tistory.com/1093
        let action = UIAction(title: "Search", handler: { _ in
            let vc = self.storyboard?.instantiateViewController(withIdentifier: "ResultView") as! ResultViewController
            vc.modalTransitionStyle = UIModalTransitionStyle.coverVertical
            vc.originImage = self.preview.image;
            self.present(vc, animated: true)
        })
        
        // 2. button
        let button = UIButton(type: .custom, primaryAction: action)
        button.frame = CGRect(x: 0, y: 0, width: 80, height: 40)
        button.backgroundColor = UIColor.red
        // 외곽 테두리로 겹쳐진 뷰 자르기
        // https://m.blog.naver.com/PostView.nhn?blogId=sungho0459&logNo=220966978686&proxyReferer=https:%2F%2Fwww.google.com%2F
        button.layer.masksToBounds = true
        button.layer.cornerRadius = 20.0
        button.layer.position = position
        
        return button
    }
    
    private func buildDrawModeLabel() -> UILabel {
        let status: UILabel = UILabel(frame: CGRect(x: 0, y: 0, width: 100, height: 40))
        status.backgroundColor = UIColor.gray
        status.layer.masksToBounds = true
        status.textAlignment = .center
        status.textColor = UIColor.white
        status.layer.cornerRadius = 20.0
        status.layer.position = CGPoint(x: self.view.frame.width * 0.5, y: gap)
        
        return status
    }
    
    private func buildDrawModeButton(position: CGPoint) -> UIButton {
        let action = UIAction(title: "Draw mode", handler: { _ in self.touchToSelectDrawMode = self.updateDrawMode()})
        let button = UIButton(type: .custom, primaryAction: action)
        button.frame = CGRect(x: 0, y: 0, width: 120, height: 40)
        button.backgroundColor = UIColor.orange
        button.layer.masksToBounds = true
        button.setTitleColor(UIColor.white, for: .normal)
        button.layer.cornerRadius = 20.0
        button.layer.position = position
        
        return button
    }
    
    private func updateDrawMode() -> Int {
        let sizeOfDrawMode: Int = 4
        return (self.touchToSelectDrawMode + 1) % sizeOfDrawMode
    }
}
