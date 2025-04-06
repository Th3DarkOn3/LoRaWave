import { ComponentFixture, TestBed, waitForAsync } from '@angular/core/testing';

import { AddDeviceFormComponent } from './add-device-form.component';

describe('AddDeviceFormComponent', () => {
  let component: AddDeviceFormComponent;
  let fixture: ComponentFixture<AddDeviceFormComponent>;

  beforeEach(waitForAsync(() => {
    TestBed.configureTestingModule({
      imports: [AddDeviceFormComponent],
    }).compileComponents();

    fixture = TestBed.createComponent(AddDeviceFormComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  }));

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
