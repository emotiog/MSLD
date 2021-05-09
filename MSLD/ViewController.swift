//
//  ViewController.swift
//  MSLD
//
//  Created by junsang on 2021/01/09.
//

import UIKit
import AVFoundation

class ViewController: UIViewController, AVCaptureVideoDataOutputSampleBufferDelegate {
    
    var preview = UIImageView(frame: CGRect( x: 0,
                                             y: 0,
                                             width: UIScreen.main.bounds.size.width,
                                             height: UIScreen.main.bounds.size.height))
    private var captureSession: AVCaptureSession = AVCaptureSession()
    private let videoDataOutput = AVCaptureVideoDataOutput()
    
    private let fps: UILabel = UILabel(frame: CGRect(x: 0, y: 0, width: 100, height: 40))
    private var startTime: CFTimeInterval!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        self.buildPreview()
        self.buildFPS()
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
        
        let imageWithLaneOverlay = OpenCVWrapper.makeGrayImage(image)
        
        DispatchQueue.main.async {
            self.preview.image = imageWithLaneOverlay
            self.fps.text = String(format:"%.1f, fps", 1 / Double(CACurrentMediaTime() - self.startTime))
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
    
    private func buildPreview() {

        preview.contentMode = UIView.ContentMode.scaleAspectFit
        preview.layer.position = CGPoint(x: self.view.frame.width * 0.5,
                                         y: self.view.frame.height * 0.5)
//        preview.frame = CGRect(x:0, y:0, width: 480, height: 640)
        view.addSubview(preview)
    }
    
    private func buildFPS() {
        fps.backgroundColor = UIColor.clear
        fps.textAlignment = .left
        fps.text = "Default"
        fps.textColor = UIColor.green
        fps.layer.position = CGPoint(x: self.view.frame.width * 0.8971,
                                     y: UIScreen.main.bounds.size.height * 0.5 - 340)
        view.addSubview(fps)
        startTime = CACurrentMediaTime()
    }
}
